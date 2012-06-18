/*
  toolfactory.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
  @file
  This file is part of the GammaRay Plugin API and declares the ToolFactory abstract base class.

  @brief
  Declares the ToolFactory abstract base class.

  @author Volker Krause \<volker.krause@kdab.com\>
*/

#ifndef GAMMARAY_TOOLFACTORY_H
#define GAMMARAY_TOOLFACTORY_H

#include <QtCore/QMetaType>
#include <QtCore/QStringList>
#include <QtCore/QtPlugin>

namespace GammaRay {

class ProbeInterface;

/**
 * @brief An abstract interface for probe tools.
 *
 * The ToolFactory class is an abstract base class for creating probe tools
 * for GammaRay.  Each tool must have a unique identifier.
 */
class ToolFactory
{
  public:
    virtual inline ~ToolFactory() {}

    /**
     * Unique id of this tool
     */
    virtual QString id() const = 0;

    /**
     * Human readable name of this tool.
     */
    virtual QString name() const = 0;

    /**
     * Class names of types this tool can handle.
     * The tool will only be activated if an object of one of these types
     * is seen in the probed application.
     */
    virtual QStringList supportedTypes() const = 0;

    /**
     * Initialize the tool.
     * Implement this method to do non-GUI initialization, such as creating
     * object tracking models etc.
     * @param probe The probe interface allowing access to the object models.
     */
    virtual void init(ProbeInterface *probe) = 0;

    /**
     * Create the UI part of this tool.
     * @param probe The probe interface allowing access to the object models.
     * @param parentWidget The parent widget for the visual elements of this tool.
     */
    virtual QWidget *createWidget(ProbeInterface *probe, QWidget *parentWidget) = 0;
};

/**
 * @brief A templated generic ToolFactory for some data type and Tool.
 */
template <typename Type, typename Tool>
class StandardToolFactory : public ToolFactory
{
  public:
    virtual inline QStringList supportedTypes() const
    {
      return QStringList(Type::staticMetaObject.className());
    }

    virtual inline QString id() const
    {
      return Tool::staticMetaObject.className();
    }

    virtual inline void init(ProbeInterface *)
    {
    }

    virtual inline QWidget *createWidget(ProbeInterface *probe, QWidget *parentWidget)
    {
      return new Tool(probe, parentWidget);
    }
};

}

Q_DECLARE_INTERFACE(GammaRay::ToolFactory, "com.kdab.GammaRay.ToolFactory/1.0")
Q_DECLARE_METATYPE(GammaRay::ToolFactory *)

#endif
