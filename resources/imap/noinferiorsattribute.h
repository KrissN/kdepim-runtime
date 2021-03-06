/*
    Copyright (C) 2012  Christian Mollekopf <mollekopf@kolabsys.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef NOINFERIORSATTRIBUTE_H
#define NOINFERIORSATTRIBUTE_H

#include <attribute.h>

class NoInferiorsAttribute : public Akonadi::Attribute
{
public:
    explicit NoInferiorsAttribute(bool noInferiors = false);
    void setNoInferiors(bool noInferiors);
    bool noInferiors() const;
    QByteArray type() const override;
    Attribute *clone() const override;
    QByteArray serialized() const override;
    void deserialize(const QByteArray &data) override;

private:
    bool mNoInferiors;
};

#endif // NOINFERIORSATTRIBUTE_H
