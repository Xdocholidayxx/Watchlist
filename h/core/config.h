#pragma once

#include <exception>
#include <string>

#include <QDir>
#include <QString>
#include <QMap>
#include <QFile>
#include <QCoreApplication>

class ConfigError : public std::exception {
    public:
        ConfigError(const std::string& message) : message(message) {}
        const char* what() const noexcept override {
            return message.c_str();
        }
    private:
        std::string message;
};

class Config {
    public:
        static Config& getInstance() {
            static Config instance;
            return instance;
        }

        QString get(const QString& key, bool throwOnNotFound = false) const {
            if (!_config.contains(key)) {
                if (throwOnNotFound) {
                    throw ConfigError("Key not found in config: " + key.toStdString());
                }
                return "";
            }
            return _config[key];
        }

        QString findStyleSheet(const QString& styleName) {
            qDebug() << "Stylesheet: Searching for stylesheet file...";

            // Construct the full filename by appending .qss to the provided styleName
            QString fileName = styleName + ".qss";

            QStringList searchPaths = {
                // First check relative to executable directory (development)
                QCoreApplication::applicationDirPath() + "/styles/" + fileName,
                QCoreApplication::applicationDirPath() + "/../resources/styles/" + fileName,
                
                // Then check installed location
                "/usr/share/App/styles/" + fileName,

                // Fallback to current directory (development)
                fileName
            };

            for (const QString& path : searchPaths) {
                qDebug() << "Stylesheet: Checking path:" << path;
                QFile file(path);
                if (file.exists()) {
                    qDebug() << "Stylesheet: Found at:" << path;
                    return path;  // Return the found path immediately
                }
            }

            // Log error if file is not found and return an empty string
            qWarning() << "Stylesheet file not found. Searched in:";
            for (const QString& path : searchPaths) {
                qWarning() << "  " << path;
            }

            return "";  // Return an empty string if not found
        }



    private:
        QMap<QString, QString> _config;

        QString findConfigFile() {
            qDebug() << "Config: Searching for config file...";
            QStringList searchPaths = {
                // First check relative to executable directory (development)
                QCoreApplication::applicationDirPath() + "/config.yaml",
                QCoreApplication::applicationDirPath() + "/../resources/config.yaml",
                
                // Then check installed location
                "/opt/Watchlist/etc/Watchlist/config.yaml",
                
                // Fallback to current directory (development)
                "config.yaml"
            };

            QString foundPath;
            for (const QString& path : searchPaths) {
                qDebug() << "Config: Checking path:" << path;
                QFile file(path);
                if (file.exists()) {
                    qDebug() << "Config: Found at:" << path;
                    foundPath = path;
                    break;
                }
            }

            if (foundPath.isEmpty()) {
                QString errorMsg = "Config file not found. Searched in:\n";
                for (const QString& path : searchPaths) {
                    errorMsg += "  " + path + "\n";
                }
                throw ConfigError(errorMsg.toStdString());
            }

            return foundPath;
        }

        QMap<QString, QString> parseConfigFile() {
            QString configPath = findConfigFile();

            QFile file(configPath);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                throw ConfigError("Failed to open config file: " + configPath.toStdString());
            }

            QMap<QString, QString> configData;
            while (!file.atEnd()) {
                QString line = file.readLine();
                if (line.startsWith("#")) {
                    continue;
                }

                QStringList parts = line.split(":");
                if (parts.size() != 2) {
                    continue;  // Skip invalid lines instead of throwing
                }

                QString key = parts[0].trimmed();
                QString value = parts[1].trimmed();
                configData[key] = value;
            }

            if (configData.isEmpty()) {
                throw ConfigError("No valid configuration entries found in: " + configPath.toStdString());
            }

            return configData;
        }

        Config() {
            qDebug() << "Config: Starting initialization";
            qDebug() << "Config: Application path:" << QCoreApplication::applicationDirPath();
            _config = parseConfigFile();
            qDebug() << "Config: Initialization complete";
        }
        ~Config() = default;
        Config(const Config&) = delete;
        Config& operator=(const Config&) = delete;
};