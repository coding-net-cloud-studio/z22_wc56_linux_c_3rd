---
--- 用于创建示例应用程序数据库的SQL
---

-- 删除任何现有的表
-- 如果需要,取消注释以下行以删除表.
-- drop table cd;
-- drop table artist;
-- drop table track;

-- 创建CD表
-- id字段为自动递增的主键
-- title字段为CD的名称,不能为空
-- artist_id字段为艺术家的ID,作为外键关联artist表
-- catalogue字段为CD的目录编号
create table cd (
    id INTEGER AUTO_INCREMENT NOT NULL PRIMARY KEY,
    title VARCHAR(70) NOT NULL,
    artist_id INTEGER NOT NULL, -- 关联artist表的id字段
    catalogue VARCHAR(30) NOT NULL
);

-- 创建艺术家表
-- id字段为自动递增的主键
-- name字段为艺术家的名称,不能为空
create table artist (
    id INTEGER AUTO_INCREMENT NOT NULL PRIMARY KEY,
    name VARCHAR(100) NOT NULL
);

-- 创建曲目表
-- cd_id字段为CD的ID,与cd表的id字段关联
-- track_id字段为曲目的唯一标识符
-- title字段为曲目的名称
-- 主键由cd_id和track_id组成,表示特定CD上的特定曲目
create table track (
    cd_id INTEGER NOT NULL, -- 关联cd表的id字段
    track_id INTEGER NOT NULL,
    title VARCHAR(70),
    PRIMARY KEY(cd_id, track_id)
);
