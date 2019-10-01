-- ДЛЯ ИНОСТРАНЦЕВ
DO $BODY$
  BEGIN
    CREATE TABLE GrantDocTypes
    (
        id serial primary key,
        grantDocTypeName TEXT
    );

    INSERT INTO GrantDocTypes (id, grantDocTypeName) VALUES(1, 'Виза');
    INSERT INTO GrantDocTypes (id, grantDocTypeName) VALUES(2, 'Вид на жительство');
    INSERT INTO GrantDocTypes (id, grantDocTypeName) VALUES(3, 'Разрешение на временное проживание');
EXCEPTION
    WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
END;
$BODY$;

DO $BODY$
  BEGIN
    CREATE TABLE VisitGoals
    (
        id serial primary key,
        visitGoalsNames VARCHAR(12)
    );

    INSERT INTO VisitGoals (id, visitGoalsNames) VALUES(1, 'cлужебная');
    INSERT INTO VisitGoals (id, visitGoalsNames) VALUES(2, 'туризм');
    INSERT INTO VisitGoals (id, visitGoalsNames) VALUES(3, 'деловая');
    INSERT INTO VisitGoals (id, visitGoalsNames) VALUES(4, 'учеба');
    INSERT INTO VisitGoals (id, visitGoalsNames) VALUES(5, 'работа');
    INSERT INTO VisitGoals (id, visitGoalsNames) VALUES(6, 'частная');
    INSERT INTO VisitGoals (id, visitGoalsNames) VALUES(7, 'транзит');
    INSERT INTO VisitGoals (id, visitGoalsNames) VALUES(9, 'гуманитарная');
    INSERT INTO VisitGoals (id, visitGoalsNames) VALUES(10, 'другая');
    COMMENT ON TABLE VisitGoals IS 'Цели визита';

EXCEPTION
    WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
END;
$BODY$;


DO $BODY$
  BEGIN
    CREATE TABLE ActivityKind
    (
        id serial primary key,
        name VARCHAR(34),
        code VARCHAR(8)
    );
    COMMENT ON TABLE ActivityKind IS 'Виды деятельности(ОКВЭД)';

EXCEPTION
    WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
END;
$BODY$;

DO $BODY$
  BEGIN
    CREATE TABLE AddressObjectType
    (
        id serial primary key,
        name VARCHAR(34),
        KLADR_TYPE_CODE VARCHAR(8)
    );

--    INSERT INTO AddressObjectType(id, name, KLADR_TYPE_CODE) VALUES(
EXCEPTION
    WHEN others THEN RAISE NOTICE '% %', SQLERRM, SQLSTATE;
END;
$BODY$;

ALTER TABLE persons_profiles  ADD COLUMN citizenhood VARCHAR(45); --гражданство
ALTER TABLE persons_profiles  ADD COLUMN grantDocType integer REFERENCES GrantDocTypes (id);
ALTER TABLE persons_profiles  ADD COLUMN grantDocserial numeric;
ALTER TABLE persons_profiles  ADD COLUMN grantDocnumber numeric;
ALTER TABLE persons_profiles  ADD COLUMN grantDocCreateDate DATE;
ALTER TABLE persons_profiles  ADD COLUMN grantDocActualDate DATE;
ALTER TABLE persons_profiles  ADD COLUMN professional VARCHAR(23);
