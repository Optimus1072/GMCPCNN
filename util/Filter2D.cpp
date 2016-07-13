//
// Created by wrede on 11.07.16.
//

#include <cmath>
#include "Filter2D.h"
#include "FileIO.h"

namespace util
{
    Filter2D::Filter2D()
    {
        multiplier_ = 1.0;
        mask_.push_back(1.0);
        dimension_ = 1;
        vicinity_ = 0;
    }

    Filter2D::Filter2D(double multiplier, std::vector<double> mask)
    {
        multiplier_ = multiplier;
        mask_ = mask;
        dimension_ = Dimension(static_cast<int>(mask_.size()));
        vicinity_ = Vicinity(static_cast<int>(mask_.size()));
    }

    Filter2D::Filter2D(std::string const & mask_string, char delimiter)
    {
        if (mask_string.empty())
        {
            multiplier_ = 1.0;
            mask_.push_back(1.0);
            dimension_ = 1;
            vicinity_ = 0;
        }
        else
        {
            std::vector<std::string> parts = FileIO::Split(mask_string, delimiter);

            multiplier_ = stof(parts[0]);

            for (int i = 1; i < parts.size(); ++i)
            {
                mask_.push_back(stod(parts[i]));
            }

            dimension_ = Dimension(static_cast<int>(mask_.size()));
            vicinity_ = Vicinity(static_cast<int>(mask_.size()));
        }
    }

    int Filter2D::Index(int x, int y) const
    {
        return y * dimension_ + x;
    }

    int Filter2D::Vicinity(int count) const
    {
        return static_cast<int>(std::sqrt(count) / 2);
    }

    int Filter2D::Dimension(int size) const
    {
        return static_cast<int>(std::sqrt(size));
    }

    double Filter2D::Get(int x, int y) const
    {
        return mask_[Index(x, y)];
    }

    double Filter2D::Get(int i) const
    {
        return mask_[i];
    }

    double Filter2D::GetMultiplier() const
    {
        return multiplier_;
    }

    int Filter2D::GetVicinity() const
    {
        return vicinity_;
    }

    int Filter2D::GetDimension() const
    {
        return dimension_;
    }

    void Filter2D::Normalize()
    {
        for (auto & value : mask_)
            value *= multiplier_;

        multiplier_ = 1.0;
    }
}
