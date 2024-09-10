#pragma once

#include <json/value.h>

#ifdef _WIN32

struct WebManager
{

    explicit WebManager() = default;

    bool Connect(std::string host, std::string user, std::string pass, std::string database)
    {
        return false;
    }

    std::string getUsername(std::string &gameKey)
    {
        return "Not Found";
    }

    void UpdateProgress(std::string &keyData, Json::Value const &achievementData)
    {
    }

    void Disconnect()
    {
    }

};

#else

#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>

// "localhost:3306", "root", "", "webscythe"

struct WebManager
{

    explicit WebManager() = default;

    bool Connect(std::string host, std::string user, std::string pass, std::string database)
    {
        bool IsConnected{false};
        try
        {
            sql::Driver *driver;

            std::cout << "\n *** TRY CONNECTION *** \n";

            /* Create a connection */
            driver = get_driver_instance();
            con.reset(driver->connect(host, user, pass));
            /* Connect to the MySQL test database */
            con->setSchema(database);

            IsConnected = true;

            std::cout << "\n *** CONNECTED *** \n";
        }
        catch (sql::SQLException &e)
        {
            std::cout << "\n *** ERROR CONEXION *** \n";
            std::cout << "# ERR: SQLException in " << __FILE__;
            std::cout << "# ERR: " << e.what();
            std::cout << " (MySQL error code: " << e.getErrorCode();
            std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        }

        return IsConnected;
    }

    std::string getUsername(std::string &gameKey)
    {

        stmt.reset(con->createStatement());
        res.reset(stmt->executeQuery("SELECT Usuario FROM partidas_usuario where p.IDKeyPartida = " + gameKey + ";"));
        std::string username{"Not Found"};
        while (res->next())
        {
            username = res->getString("Usuario");
        }
        std::cout << "\n*** Username Loaded: " << username << " ***\n";

        return username;
    }

    void UpdateProgress(std::string &keyData, Json::Value const &achievementData)
    {
        if (HasSpaceBetween(keyData))
            throw "*** ERROR: The key contains an invalid character ***";

        stmt.reset(con->createStatement());
        res.reset(stmt->executeQuery("SELECT IDKeyPartida FROM partidas WHERE IDKeyPartida = " + keyData + ";"));
        if (not res->first())
            UploadKeyData(keyData);
        // Achievement Data
        for (auto const &achievement : achievementData)
        {
            if (achievement["Obtained"].asBool())
            {

                std::string achievementID = achievement["AchievementID"].asString();
                if (HasSpaceBetween(keyData))
                    throw "*** ERROR: A Achievement contains an invalid character ***";

                std::string date = achievement["Date"].asString();
                if (HasSpaceBetween(keyData))
                    throw "*** ERROR: A date contains an invalid character ***";

                float timeSeconds = achievement["Time"].asFloat();

                // Time in Hours
                std::string time = std::to_string(timeSeconds / 3600.0f);

                res.reset(stmt->executeQuery("SELECT IDKeyPartida FROM partida_logros WHERE IDKeyPartida = " + keyData + " AND IDLogro = '" + achievementID + "';"));

                if (not res->first())
                {
                    res.reset(stmt->executeQuery("SELECT IDLogro FROM logros WHERE IDLogro = '" + achievementID + "';"));

                    if (res->first())
                    {
                        stmt->execute("INSERT INTO partida_logros (IDKeyPartida, IDLogro, FechaConseguido, TiempoTotal) VALUES (" + keyData + ",'" + achievementID + "','" + date + "'," + time + ");");
                    }
                    else
                        throw "*** ERROR: Achievement Is Not on DataBase ***";
                }
            }
        }
    }

    void Disconnect()
    {
        res.reset(nullptr);
        stmt.reset(nullptr);
        con.reset(nullptr);
    }

private:
    std::unique_ptr<sql::Connection> con{nullptr};
    std::unique_ptr<sql::Statement> stmt{nullptr};
    std::unique_ptr<sql::ResultSet> res{nullptr};

    void UploadKeyData(std::string const &keyData) const noexcept
    {
        stmt->execute("INSERT INTO partidas (IDKeyPartida) VALUES (" + keyData + ");");
    }

    constexpr bool HasSpaceBetween(std::string const &value) const noexcept
    {
        return (value.find(" ") != std::string::npos || value.find(";") != std::string::npos);
    }
};

#endif