--
-- 创建名为children的表
-- 用于存储儿童的信息,包括儿童编号,名字和年龄
-- 儿童编号是主键,自动递增
CREATE TABLE
  children (
    childno int (11) NOT NULL auto_increment, -- 儿童编号,唯一标识每个儿童
    fname varchar(30), -- 儿童的名字
    age int (11), -- 儿童的年龄
    PRIMARY KEY (childno) -- 指定儿童编号为主键
  );

--
-- 向'children'表中插入数据
-- 插入多条儿童记录,每条记录包含儿童编号,名字和年龄
INSERT INTO
  children (childno, fname, age)
VALUES
  (2, 'Andrew', 17);

-- 插入编号为2,名字为Andrew,年龄为17的儿童记录
INSERT INTO
  children (childno, fname, age)
VALUES
  (3, 'Gavin', 9);

-- 插入编号为3,名字为Gavin,年龄为9的儿童记录
INSERT INTO
  children (childno, fname, age)
VALUES
  (4, 'Duncan', 6);

-- 插入编号为4,名字为Duncan,年龄为6的儿童记录
INSERT INTO
  children (childno, fname, age)
VALUES
  (5, 'Emma', 4);

-- 插入编号为5,名字为Emma,年龄为4的儿童记录
INSERT INTO
  children (childno, fname, age)
VALUES
  (6, 'Alex', 15);

-- 插入编号为6,名字为Alex,年龄为15的儿童记录
INSERT INTO
  children (childno, fname, age)
VALUES
  (7, 'Adrian', 9);

-- 插入编号为7,名字为Adrian,年龄为9的儿童记录
