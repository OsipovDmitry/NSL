#ifndef KCAL_H
#define KCAL_H

namespace model
{

inline double calculateKcal(double protein, double fat, double carb)
{
    return 4. * protein + 9. * fat + 4. * carb;
}

}

#endif // KCAL_H
