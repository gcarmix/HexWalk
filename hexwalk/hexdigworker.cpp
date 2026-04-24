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
#include "hexdigworker.h"

#include <exception>
#include <filesystem>

#include "scanner.hpp"

HexdigWorker::HexdigWorker(const QString &filePath,
                           bool enableExtraction,
                           int recursionDepth,
                           const QString &extractionPath,
                           bool verbose,
                           QObject *parent)
    : QThread(parent),
      m_filePath(filePath),
      m_enableExtraction(enableExtraction),
      m_recursionDepth(recursionDepth),
      m_extractionPath(extractionPath),
      m_verbose(verbose)
{
}

void HexdigWorker::run()
{
    try {
        std::filesystem::path extractPath =
            m_extractionPath.isEmpty() ? std::filesystem::path("extractions/")
                                       : std::filesystem::path(m_extractionPath.toStdString());

        Scanner scanner(m_enableExtraction, m_recursionDepth, 0, extractPath, m_verbose);
        results = scanner.scan(std::filesystem::path(m_filePath.toStdString()));
        emit scanFinished();
    } catch (const std::exception &e) {
        errorMessage = QString::fromStdString(e.what());
        emit scanFailed(errorMessage);
    } catch (...) {
        errorMessage = QStringLiteral("Unknown error during HexDig scan");
        emit scanFailed(errorMessage);
    }
}