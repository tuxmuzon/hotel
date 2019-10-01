ALTER TABLE persons_profiles  ADD COLUMN is_vip BOOLEAN;
ALTER TABLE persons_profiles  ADD COLUMN vip_price money;
ALTER TABLE bookings  ADD COLUMN discont_money money;
ALTER TABLE bookings  ADD COLUMN type integer DEFAULT 0; -- тип 1 - груповые брони.




