#ifndef BLACKMISC_GEOLATITUDE_H
#define BLACKMISC_GEOLATITUDE_H
#include "blackmisc/geoearthangle.h"

namespace BlackMisc
{
namespace Geo
{

/*!
 * \brief Latitude
 */
class CLatitude : public CEarthAngle<CLatitude>
{
protected:
    /*!
     * \brief Specific string representation
     */
    virtual QString stringForConverter() const
    {
        QString s = "latitude ";
        return s.append(CEarthAngle::stringForConverter());
    }

public:
    /*!
     * \brief Default constructor
     */
    CLatitude() : CEarthAngle() {}

    /*!
     * \brief Copy constructor
     * \param latitude
     */
    CLatitude(const CLatitude &latitude) : CEarthAngle(latitude) {}

    /*!
     * \brief Init by double value
     * \param value
     * \param unit
     */
    CLatitude(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit): CEarthAngle(value, unit) {}

    /*!
     * \brief Virtual destructor
     */
    virtual ~CLatitude() {}
};

} // namespace
} // namespace


#endif // guard
