-- деньги в рубли
-- в postgresql.conf
-- lc_monetary = 'ru_RU.UTF-8'
-- проверка
-- select 10::money;

DO $BODY$
  BEGIN
    create database mis;
    create user mis with encrypted password 'mis';
    grant all privileges on database mis to mis;
  EXCEPTION
    WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
END;
$BODY$;

    CREATE EXTENSION IF NOT EXISTS ltree;

    DO $BODY$
      BEGIN
        CREATE TABLE access_users
        (
            id serial primary key,
            shortname VARCHAR(25),
            printname VARCHAR(25),
            vat_number VARCHAR(25),
            login VARCHAR(25),
            password TEXT,
            admin boolean,
            created timestamp(0) without time zone NOT NULL DEFAULT ('now'::text)::timestamp without time zone
        );
        COMMENT ON TABLE access_users IS 'Пользователи';
        ALTER TABLE access_users OWNER TO mis;
        INSERT INTO access_users (id, shortname, printname,login, admin) VALUES(1, 'Сергиенко В', 'Сергиенко В','sergienko', true);
        INSERT INTO access_users (id, shortname, printname,login, admin) VALUES(2, 'Погорельская Н.В.', 'Киселева О.К.','kiseleva', true);
        INSERT INTO access_users (id, shortname, printname,login, admin) VALUES(3, 'Погорельская Н.В.', 'Максимова Л.В.','maksimova', true);
        INSERT INTO access_users (id, shortname, printname,login, admin) VALUES(0, 'Администратор', 'Администратор','admin', true);
    EXCEPTION
        WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
    END;
    $BODY$;


DO $BODY$
  BEGIN
    CREATE TABLE "sale_crater" (
            "id" serial primary key,
            "title" character varying(30) NOT NULL,
            "order_id" integer
    );
    COMMENT ON TABLE sale_crater IS 'Воронки продаж';
    ALTER TABLE sale_crater OWNER TO mis;
EXCEPTION
    WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
END;
$BODY$;

DO $BODY$
  BEGIN
  INSERT INTO sale_crater (id, title, order_id)  VALUES(1, 'Контакт', 1);
  INSERT INTO sale_crater (id, title, order_id)  VALUES(2, 'Потенциальный', 2);
  INSERT INTO sale_crater (id, title, order_id)  VALUES(3, 'Клиент', 3);
  INSERT INTO sale_crater (id, title, order_id)  VALUES(4, 'Постоянный',4 );
  INSERT INTO sale_crater (id, title, order_id)  VALUES(5, 'Бывший',5 );
  INSERT INTO sale_crater (id, title, order_id)  VALUES(6, 'Черный Список',6 );
EXCEPTION
    WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
END;
$BODY$;

    DO $BODY$
      BEGIN
        CREATE TABLE persons_profiles
        (
            id serial primary key,
            firstname VARCHAR(20),
            secondname VARCHAR(20),
            lastname VARCHAR(20),
            mobile_phone VARCHAR(20),
            email VARCHAR(25),
            beathday DATE,
            sex VARCHAR(1),
            beath_country text,
            beath_place text,
            doc_type VARCHAR(22),
            doc_serial VARCHAR(11),
            doc_number VARCHAR(11),
            doc_create_date DATE,
            doc_create_org text,
            doc_create_org_code VARCHAR(8),
            resident_country text,
            address  text,
            comment text,
            --Кто привел апдейтить таблицу
            referal_id integer REFERENCES persons_profiles (id) NULL,
            creator integer REFERENCES access_users (id) NULL,
            sale_crater_id integer REFERENCES sale_crater (id) NULL,
            created timestamp(0) without time zone NOT NULL DEFAULT ('now'::text)::timestamp without time zone
        );
        COMMENT ON TABLE persons_profiles IS 'Клиенты';
        ALTER TABLE persons_profiles OWNER TO mis;
      EXCEPTION
        WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
      END;
    $BODY$;

    INSERT INTO persons_profiles (id, firstname) VALUES(0, '-----');

    DO $BODY$
      BEGIN
        CREATE EXTENSION IF NOT EXISTS ltree;
        CREATE TABLE IF NOT EXISTS apartments
        (
            id serial primary key,
            shortname VARCHAR(25),
            parent_id integer,
            type VARCHAR(12),
            photodata bytea,
            creator integer REFERENCES access_users (id) NULL,
            created timestamp(0) without time zone NOT NULL DEFAULT ('now'::text)::timestamp without time zone,
            path ltree
        );
        CREATE INDEX apartments_path_gist_idx ON apartments USING GIST (path);
        COMMENT ON TABLE apartments IS 'Номерной фонд';
        ALTER TABLE apartments OWNER TO mis;
      EXCEPTION
          WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
      END;
    $BODY$;

    CREATE OR REPLACE FUNCTION public.gen_path_trigger()
        RETURNS trigger
        LANGUAGE 'plpgsql'
        COST 100
        VOLATILE NOT LEAKPROOF
    AS $BODY$
     BEGIN
      IF (TG_OP = 'INSERT') THEN
        EXECUTE 'UPDATE '||TG_TABLE_NAME||' SET path=COALESCE((SELECT path||'||NEW.id||'::text FROM '||TG_TABLE_NAME||' WHERE id='||COALESCE(NEW.parent_id,-1)||'),'||NEW.id||'::text::ltree) WHERE id='||NEW.id;
      ELSIF (TG_OP = 'UPDATE') THEN
        EXECUTE 'UPDATE '||TG_TABLE_NAME||' SET path=COALESCE((SELECT path||'||NEW.id||'::text FROM '||TG_TABLE_NAME||' WHERE id='||COALESCE(NEW.parent_id,-1)||'),'||NEW.id||'::text::ltree) WHERE id='||NEW.id;
        EXECUTE 'WITH RECURSIVE d AS (
          SELECT id,COALESCE(parent_id,0) AS parent_id,path::text AS tid
          FROM '||TG_TABLE_NAME||' WHERE id = '||NEW.id||'
        UNION ALL
          SELECT c.id,COALESCE(c.parent_id,0) AS parent_id,tid||''.''||c.id::text
          FROM d JOIN '||TG_TABLE_NAME||' c ON c.parent_id = d.id
        )
        UPDATE '||TG_TABLE_NAME||' SET path=paths.tid FROM
        (SELECT id,tid::ltree FROM d
        GROUP BY id,tid) AS paths WHERE '||TG_TABLE_NAME||'.id = paths.id;';
      END IF;
      RETURN NEW;
     END;
    $BODY$;

    ALTER FUNCTION public.gen_path_trigger() OWNER TO mis;

    DO $BODY$
      BEGIN
        CREATE TRIGGER  apartments_path_insert_trigger
            AFTER INSERT
            ON public.apartments
            FOR EACH ROW
            EXECUTE PROCEDURE public.gen_path_trigger();

        -- Trigger: apartments_path_update_trigger

        -- DROP TRIGGER apartments_path_update_trigger ON public.apartments;


        CREATE TRIGGER apartments_path_update_trigger
            AFTER UPDATE OF parent_id
            ON public.apartments
            FOR EACH ROW
            WHEN ((old.parent_id IS DISTINCT FROM new.parent_id))
            EXECUTE PROCEDURE public.gen_path_trigger();
      EXCEPTION
          WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
      END;
    $BODY$;


    DO $BODY$
      BEGIN
        INSERT INTO apartments (id, shortname, parent_id) VALUES(1, 'Весь Отель', NULL);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(2, 'Номер 1', 1);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(3, 'Койка 1(Номер 1)', 2);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(4, 'Койка 2(Номер 1)', 2);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(5, 'Номер 2', 1);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(6, 'Номер 3', 1);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(7, 'Койка 1(Номер 3)', 6);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(8, 'Койка 2(Номер 3)', 6);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(9, 'Койка 3(Номер 3)', 6);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(10, 'Койка 4(Номер 3)', 6);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(11, 'Номер 4', 1);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(12, 'Койка 1(Номер 4)', 11);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(13, 'Койка 2(Номер 4)', 11);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(14, 'Койка 3(Номер 4)', 11);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(15, 'Номер 5', 1);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(16, 'Койка 1(Номер 5)', 15);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(17, 'Койка 2(Номер 5)', 15);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(18, 'Койка 3(Номер 5)', 15);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(19, 'Койка 4(Номер 5)', 15);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(20, 'Койка 5(Номер 5)', 15);
        INSERT INTO apartments (id, shortname, parent_id) VALUES(21, 'Койка 6(Номер 5)', 15);
    EXCEPTION
        WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
    END;
    $BODY$;

    -- вывести всех потомков
    --SELECT shortname FROM apartments WHERE path <@ '1.2.3';
    -- вывести всех  предков
    --SELECT shortname FROM apartments WHERE parent_id @> '1.2.3';


    DO $BODY$
      BEGIN
        CREATE TABLE prices
        (
            id serial primary key,
            shortname VARCHAR(22),
            apartment_id integer REFERENCES Apartments (id) NULL,
            start_date DATE NOT NULL DEFAULT ('now'::text)::timestamp without time zone,
            stop_date DATE DEFAULT ('infinity'::text)::timestamp without time zone,
            cost money,
            for_children boolean DEFAULT false,
            created timestamp(0) without time zone NOT NULL DEFAULT ('now'::text)::timestamp without time zone
        );
        COMMENT ON TABLE prices IS 'Цены';
        ALTER TABLE prices OWNER TO mis;
        INSERT INTO prices (apartment_id, cost ) VALUES(2, 1900);
        INSERT INTO prices (apartment_id, cost, for_children) VALUES(2, 980, true);
        INSERT INTO prices (apartment_id, cost ) VALUES(5, 1800);
        INSERT INTO prices (apartment_id, cost ) VALUES(6, 2600);
        INSERT INTO prices (apartment_id, cost, for_children) VALUES(6, 650, true);
        INSERT INTO prices (apartment_id, cost ) VALUES(11, 2050);
        INSERT INTO prices (apartment_id, cost, for_children) VALUES(11, 690, true);
        INSERT INTO prices (apartment_id, cost ) VALUES(15, 3650);
        INSERT INTO prices (apartment_id, cost, for_children) VALUES(15, 650, true);
    EXCEPTION
        WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
    END;
    $BODY$;

-- добавить сюда всех детишек

    DO $BODY$
      BEGIN
        CREATE TABLE advertising_source
        (
            id serial primary key,
            shortname VARCHAR(22),
            phonenumber VARCHAR(11)
        );
        COMMENT ON TABLE advertising_source IS 'Рекламный источник';
        ALTER TABLE advertising_source OWNER TO mis;
        INSERT INTO advertising_source (shortname) VALUES('Рекомендация'); --с возможностью выбора рекомендущего, все актуально для новыйх броней
        INSERT INTO advertising_source (shortname) VALUES('Постоянный клиент'); --> 2 раз заехал, автоматом вешать скидку и т.д.
        INSERT INTO advertising_source (shortname) VALUES('2 ГИС');
        INSERT INTO advertising_source (shortname) VALUES('Booking');
        INSERT INTO advertising_source (shortname) VALUES('Сайт');
        INSERT INTO advertising_source (shortname) VALUES('Поиск яндекс');
        INSERT INTO advertising_source (shortname) VALUES('Поиск гугол');
    EXCEPTION
        WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
    END;
    $BODY$;

    DO $BODY$
      BEGIN
        CREATE TABLE booking_status
        (
            id serial primary key,
             shortname VARCHAR(22)
        );

        INSERT INTO booking_status (id, shortname) VALUES(0, 'Черновик');
        INSERT INTO booking_status (id, shortname) VALUES(1, 'Забронировано');
        INSERT INTO booking_status (id, shortname) VALUES(2, 'Оплачено');
        INSERT INTO booking_status (id, shortname) VALUES(3, 'Прибыл');
        INSERT INTO booking_status (id, shortname) VALUES(4, 'Просрочено');
        INSERT INTO booking_status (id, shortname) VALUES(5, 'Долг');
        --проставлять дату на печатной форме
        INSERT INTO booking_status (id, shortname) VALUES(6, 'Выбыл');
        INSERT INTO booking_status (id, shortname) VALUES(7, 'Возврат');
        COMMENT ON TABLE booking_status IS 'Статусы броней';
        ALTER TABLE booking_status OWNER TO mis;

    EXCEPTION
        WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
    END;
    $BODY$;

    DO $BODY$
      BEGIN
        CREATE TABLE bookings
        (
            id serial primary key,
            personsProfileID integer REFERENCES persons_profiles (id),
            booking_status integer REFERENCES booking_status (id) DEFAULT 0,
            apartment_id integer REFERENCES apartments (id),
            accessUser_id integer REFERENCES access_users (id),
            advertising_source_id integer REFERENCES advertising_source (id),
            start_date timestamp,
            stop_date timestamp,
            cost money,
            discont smallint,
            total money,
            payd money,
            creator integer REFERENCES access_users (id),
            comment text,
            created timestamp(0) without time zone NOT NULL DEFAULT ('now'::text)::timestamp without time zone
        --   visitGoalsID REFERENCES VisitGoals (id),
        );

        INSERT INTO bookings (apartment_id, start_date, stop_date,  cost,  total ) VALUES(2, '2019-03-09'::timestamp, '2019-03-10', 100, 100);
        INSERT INTO bookings (apartment_id, start_date, stop_date,  cost,  total ) VALUES(3, '2019-03-29'::timestamp, '2019-03-30', 1000, 1000);

        COMMENT ON TABLE bookings IS 'Бронирование';
        ALTER TABLE bookings OWNER TO mis;

    EXCEPTION
        WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
    END;
    $BODY$;

    DO $BODY$
      BEGIN
        CREATE TABLE nomenclature
        (
           id serial primary key,
           user_code integer UNIQUE,
           printname VARCHAR(25),
           shortname text,
           cost money,
           units smallint, -- 1-шт 2-литры
           type smallint, -- 1-Товар 2-Услуга 3-Прокат -- для проката сделать таблицу отдельную.
           alc BOOLEAN DEFAULT false,
           creator integer REFERENCES access_users (id),
           comment text,
           created timestamp(0) without time zone NOT NULL DEFAULT ('now'::text)::timestamp without time zone
         );
         COMMENT ON TABLE nomenclature IS 'Прайс';
         ALTER TABLE nomenclature OWNER TO mis;

         INSERT INTO nomenclature (user_code, printname, shortname, cost, units, type, creator, comment)
            VALUES(1, 'Услуги проживания доп', 'Шампунь+Кондиционер', 20, 1, 1, 1, 'Создано автоматически');
         INSERT INTO nomenclature (user_code, printname, shortname, cost, units, type, creator, comment)
            VALUES(2, 'Услуги проживания доп', 'бритвенный набор', 35, 1, 1, 1, 'Создано автоматически');
         INSERT INTO nomenclature (user_code, printname, shortname, cost, units, type, creator, comment)
            VALUES(3, 'Услуги проживания доп', 'зубной набор', 35, 1, 1, 1, 'Создано автоматически');
         INSERT INTO nomenclature (user_code, printname, shortname, cost, units, type, creator, comment)
            VALUES(4, 'Услуги проживания доп', 'гель для душа', 20, 1, 1, 1, 'Создано автоматически');
         INSERT INTO nomenclature (user_code, printname, shortname, cost, units, type, creator, comment)
            VALUES(5, 'Услуги проживания доп', 'тапочки', 50, 1, 1, 1, 'Создано автоматически');
         INSERT INTO nomenclature (user_code, printname, shortname, cost, units, type, creator, comment)
            VALUES(6, 'Услуги проживания доп', 'полотенце махровое (ПРОКАТ)', 80, 1, 3, 1, 'Создано автоматически');
         INSERT INTO nomenclature (user_code, printname, shortname, cost, units, type, creator, comment)
            VALUES(7, 'Услуги проживания доп', 'халат фавельный (ПРОКАТ)', 100, 1, 3, 1, 'Создано автоматически');
         INSERT INTO nomenclature (user_code, printname, shortname, cost, units, type, creator, comment)
            VALUES(8, 'Услуги проживания доп', 'утюг (ПРОКАТ)', 40, 1, 3, 1, 'Создано автоматически');
         INSERT INTO nomenclature (user_code, printname, shortname, cost, units, type, creator, comment)
            VALUES(9, 'Услуги проживания доп', 'фен (ПРОКАТ)', 40, 1, 3, 1, 'Создано автоматически');
         INSERT INTO nomenclature (user_code, printname, shortname, cost, units, type, creator, comment)
            VALUES(10, 'Услуги проживания доп', 'распечатка', 5, 1, 2, 1, 'Создано автоматически');
         INSERT INTO nomenclature (user_code, printname, shortname, cost, units, type, creator, comment)
            VALUES(11, 'Услуги проживания доп', 'Стирка', 150, 1, 2, 1, 'Создано автоматически');
         INSERT INTO nomenclature (user_code, printname, shortname, cost, units, type, creator, comment)
            VALUES(12, 'Услуги проживания доп', 'Стирка со скидкой', 120, 1, 2, 1, 'Создано автоматически');
         INSERT INTO nomenclature (user_code, printname, shortname, cost, units, type, creator, comment)
            VALUES(13, 'Услуги проживания доп', 'Сувенир - закладка', 100, 1, 1, 1, 'Создано автоматически');
         INSERT INTO nomenclature (user_code, printname, shortname, cost, units, type, creator, comment)
            VALUES(14, 'Услуги проживания доп', 'Сувенир - почтовая карточка', 160, 1, 1, 1, 'Создано автоматически');




    EXCEPTION
        WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
    END;
    $BODY$;

    DO $BODY$
      BEGIN
        CREATE TABLE pay_documents
        (
           id serial primary key,
           personsProfileID integer REFERENCES persons_profiles (id) DEFAULT NULL,
           bookings_id integer REFERENCES bookings (id) DEFAULT NULL,
           accessUser_id integer REFERENCES access_users (id),
           nomenclature_id integer REFERENCES nomenclature (id) DEFAULT NULL,
           cost money,
           count real DEFAULT 0,
           payType  integer, -- 0-Cash, 1-Electro
           operation_type integer, --1  sale, 2- return, 3-openshift, 4-z-report, 5-x-report
           cash_id integer,
           shift integer,
           document_number integer,
           comment text,
           created timestamp(0) without time zone NOT NULL DEFAULT ('now'::text)::timestamp without time zone
         );
         COMMENT ON TABLE pay_documents IS 'Фискальные Документы';
         ALTER TABLE pay_documents OWNER TO mis;
    EXCEPTION
        WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
    END;
    $BODY$;

    -- для рассылок
    DO $BODY$
      BEGIN
        CREATE TABLE "inbox" (
                "id" serial primary key,
                "number" character varying(20) NOT NULL,
                "smsdate" timestamp NOT NULL,
                "insertdate" timestamp DEFAULT now() NOT NULL,
                "text" character varying(160),
                "phone" integer,
                "processed" bool DEFAULT 'false'
        );
        COMMENT ON TABLE inbox IS 'Входящие сообщения';
        ALTER TABLE inbox OWNER TO mis;
    EXCEPTION
        WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
    END;
    $BODY$;

    DO $BODY$
      BEGIN
        CREATE TABLE "outbox" (
                "id" serial primary key,
                "number" character varying(20) NOT NULL,
                "processed_date" timestamp DEFAULT now() NOT NULL,
                "insertdate" timestamp DEFAULT now() NOT NULL,
                "text" character varying(160),
                "phone" integer,
                "processed" bool DEFAULT 'false',
                "error" smallint DEFAULT '-1' NOT NULL,
                "dreport" smallint DEFAULT '0' NOT NULL,
                "not_before" time without time zone DEFAULT '00:00:00' NOT NULL,
                "not_after" time without time zone DEFAULT '23:59:59' NOT NULL
        );
        COMMENT ON TABLE outbox IS 'Исходящие сообщения';
        ALTER TABLE outbox OWNER TO mis;
    EXCEPTION
      WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
    END;
    $BODY$;

    DO $BODY$
      BEGIN
        CREATE TABLE "news" (
                "id" serial primary key,
                "text" character varying(160) NOT NULL,
                "type"  integer NOT NULL, -- 1 sms, 2 -email, 3 sms and email
                "created" timestamp NOT NULL default CURRENT_TIMESTAMP,
                "is_site" bool DEFAULT 'false',
                "start_send_date" timestamp DEFAULT now() NOT NULL,
                "stop_send_date" timestamp DEFAULT now() NOT NULL
        );
        COMMENT ON TABLE news IS 'Новости-Рассылки';
        ALTER TABLE news OWNER TO mis;
    EXCEPTION
      WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
    END;
    $BODY$;

    DO $BODY$
      BEGIN
        CREATE TABLE "mail_out" (
                "id" serial primary key,
                "persons_profiles_id" integer REFERENCES persons_profiles (id),
                "news_id"  integer REFERENCES news (id),
                "created" timestamp NOT NULL default CURRENT_TIMESTAMP,
                "outbox_id" integer REFERENCES outbox
        );
        COMMENT ON TABLE mail_out IS 'Адресная рассылка';
        ALTER TABLE mail_out OWNER TO mis;
    EXCEPTION
        WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
    END;
    $BODY$;


