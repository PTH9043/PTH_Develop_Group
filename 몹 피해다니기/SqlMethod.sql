--======================================================
-- Insert
--======================================================

USE [2018184040PTH]
GO

-- Drop stored procedure if it already exists
IF OBJECT_ID('SqlMethod','P') IS NOT NULL
	DROP PROCEDURE [SqlMethod]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE InsertCharacterData
    @ID INT,
    @Name NCHAR(50),
    @Level INT,
    @Job INT,
    @hp FLOAT,
    @mp FLOAT,
    @exp FLOAT,
    @atk FLOAT,
    @speed FLOAT,
    @luck FLOAT,
    @x INT,
    @y INT
AS
BEGIN
    INSERT INTO dbo.CharacterData(ID, Name, Level, Job, hp, mp, exp, atk, speed, luck, x, y)
    VALUES (@ID, @Name, @Level, @Job, @hp, @mp, @exp, @atk, @speed, @luck, @x, @y);
END;
--======================================================
-- Delete
--======================================================
GO
CREATE OR ALTER PROCEDURE DeleteCharacter
    @ID INT
AS
BEGIN
    DELETE FROM dbo.CharacterData WHERE ID = @ID;
END
GO
--======================================================
-- Update
--======================================================
GO
CREATE OR ALTER PROCEDURE UpdateCharacter
    @ID INT,
    @Name NVARCHAR(50),
    @Level INT,
    @Job INT,
    @HP INT,
    @MP INT,
    @Exp INT,
    @ATK INT,
    @Speed INT,
    @Luck INT,
    @X FLOAT,
    @Y FLOAT
AS
BEGIN
    UPDATE dbo.CharacterData
    SET Name = @Name,
        Level = @Level,
        Job = @Job,
        HP = @HP,
        MP = @MP,
        Exp = @Exp,
        ATK = @ATK,
        Speed = @Speed,
        Luck = @Luck,
        X = @X,
        Y = @Y
    WHERE ID = @ID;
END
GO
--======================================================
-- FindData
--======================================================
Go
CREATE OR ALTER PROCEDURE GetCharacter
    @ID INT
AS
BEGIN
    SELECT ID, Name, Level, Job, hp, mp, exp, atk, speed, luck, x, y
    FROM dbo.CharacterData
    WHERE ID = @ID;
END
--======================================================
-- GetAllCharacterData
--======================================================
GO
CREATE PROCEDURE GetAllCharacter
AS
BEGIN
	SELECT ID, Name, Level, Job, hp, mp, exp, atk, speed, luck, x, y
	FROM dbo.CharacterData
END
GO
--======================================================
-- DeleteAllCharacterData
--======================================================
GO
CREATE PROCEDURE DeleteAllCharacterData
AS
BEGIN
    DELETE FROM dbo.CharacterData;
END
GO