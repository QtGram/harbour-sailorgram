/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *      Roberto Mier
 *      Tiago Herrmann
 */

#ifndef LIBQTELEGRAM_GLOBAL_H
#define LIBQTELEGRAM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBQTELEGRAM_LIBRARY)
#  define LIBQTELEGRAMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBQTELEGRAMSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBQTELEGRAM_GLOBAL_H
