//
// Created by wrede on 06.06.16.
//

#include "Grid.h"

namespace util
{
    Grid::Grid(int width_count, int height_count, double width, double height)
            : Grid(width_count, height_count, 1, width, height, 0.0)
    {
        /* EMPTY */
    }

    Grid::Grid(int width_count, int height_count, int depth_count,
               double width, double height, double depth)
            : width_count_(width_count),
              height_count_(height_count),
              depth_count_(depth_count),
              width_(width),
              height_(height),
              depth_(depth),
              cell_width_(width / width_count),
              cell_height_(height / height_count),
              cell_depth_(depth / depth_count)
    {
        for (int z = 0; z < depth_count; ++z)
        {
            values_.push_back(std::vector<std::vector<core::ObjectDataPtr>>());

            for (int y = 0; y < height_count; ++y)
            {
                values_[z].push_back(std::vector<core::ObjectDataPtr>());

                for (int x = 0; x < width_count; ++x)
                {
                    values_[z][y].push_back(core::ObjectDataPtr());
                }
            }
        }
    }

    void Grid::PositionToIndex(double x, double y, double z,
                               int& xi, int& yi, int& zi) const
    {
        xi = (int) (x / cell_width_);
        yi = (int) (y / cell_height_);

        if (depth_count_ > 1)
        {
            zi = (int) (z / cell_depth_);
        }
        else
        {
            zi = 0;
        }
    }

    void Grid::SetValue(core::ObjectDataPtr value, int x, int y, int z)
    {
        values_[z][y][x] = value;
    }

    void Grid::SetValue(core::ObjectDataPtr value, double x, double y, double z)
    {
        int xi, yi, zi;
        PositionToIndex(x, y, z, xi, yi, zi);
        SetValue(value, xi, yi, zi);
    }

    core::ObjectDataPtr Grid::GetValue(int x, int y, int z) const
    {
        return values_[z][y][x];
    }

    core::ObjectDataPtr Grid::GetValue(double x, double y, double z) const
    {
        int xi, yi, zi;
        PositionToIndex(x, y, z, xi, yi, zi);
        return GetValue(xi, yi, zi);
    }

    int Grid::GetWidthCount() const
    {
        return width_count_;
    }

    int Grid::GetHeightCount() const
    {
        return height_count_;
    }

    int Grid::GetDepthCount() const
    {
        return depth_count_;
    }

    double Grid::GetWidth() const
    {
        return width_;
    }

    double Grid::GetHeight() const
    {
        return height_;
    }

    double Grid::GetDepth() const
    {
        return depth_;
    }
}