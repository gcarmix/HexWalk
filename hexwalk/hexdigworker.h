/*
 * Copyright (C) 2025 Carmix <carmixdev@gmail.com>
 *
 * This file is part of HexWalk.
 *
 * HexWalk is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HexWalk is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef HEXDIGWORKER_H
#define HEXDIGWORKER_H

#include <QThread>
#include <QString>
#include <vector>
#include "scanresult.hpp"

class HexdigWorker : public QThread
{
    Q_OBJECT
public:
    explicit HexdigWorker(const QString &filePath,
                          bool enableExtraction,
                          int recursionDepth,
                          const QString &extractionPath,
                          bool verbose,
                          QObject *parent = nullptr);

    std::vector<ScanResult> results;
    QString errorMessage;

signals:
    void scanFinished();
    void scanFailed(const QString &message);

protected:
    void run() override;

private:
    QString m_filePath;
    bool m_enableExtraction;
    int m_recursionDepth;
    QString m_extractionPath;
    bool m_verbose;
};

#endif // HEXDIGWORKER_H
