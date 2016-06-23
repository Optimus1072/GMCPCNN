//
// Created by wrede on 06.06.16.
//

#ifndef GBMOT_GRID_H
#define GBMOT_GRID_H

#include <vector>
#include "../core/ObjectData.h"

namespace util
{
    /**
     * Class for storing values in a three dimensional grid.
     * Can also be used for two dimensions but has a bit overhead.
     */
    class Grid
    {
    private:
        /**
         * The number of values on the x axis
         */
        const int width_count_;

        /**
         * The number of values on the y axis
         */
        const int height_count_;

        /**
         * The number of values on the z axis
         */
        const int depth_count_;

        /**
         * The size of the whole grid on the x axis
         */
        const double width_;

        /**
         * The size of the whole grid on the y axis
         */
        const double height_;

        /**
         * The size of the whole grid on the z axis
         */
        const double depth_;

        /**
         * The size of one grid cell on the x axis
         */
        const double cell_width_;

        /**
         * The size of one grid cell on the y axis
         */
        const double cell_height_;

        /**
         * The size of one grid cell on the z axis
         */
        const double cell_depth_;

        /**
         * The values stored in the grid cells
         */
        std::vector<std::vector<std::vector<core::ObjectDataPtr>>> values_;
    public:
        /**
         * Creates a new two dimensional grid.
         * @param width_count The number of elements on the x axis
         * @param height_count The number of elements on the y axis
         * @param width The size of the whole grid on the x axis
         * @param height The size of the whole grid on the y axis
         */
        Grid(int width_count, int height_count, double width, double height);

        /**
         * Creates a new three dimensional grid.
         * @param width_count The number of elements on the x axis
         * @param height_count The number of elements on the y axis
         * @param depth_count The number of elements on the z axis
         * @param width The size of the whole grid on the x axis
         * @param height The size of the whole grid on the y axis
         * @param depth The size of the whole grid on the z axis
         */
        Grid(int width_count, int height_count, int depth_count,
             double width, double height, double depth);

        /**
         * Sets a value in the grid cell with the given index.
         * @param value The value to set
         * @param x The x axis index
         * @param y The y axis index
         * @param z The z axis index
         */
        void SetValue(core::ObjectDataPtr value, int x, int y, int z = 0);

        /**
         * Sets a value in the grid cell at the given position.
         * @param value The value to set
         * @param x The x axis value
         * @param y The y axis value
         * @param z The z axis value
         */
        void SetValue(core::ObjectDataPtr value,
                      double x, double y, double z = 0);

        /**
         * Gets the value in the grid cell with the given index.
         * @param x The x axis index
         * @param y The y axis index
         * @param z The z axis index
         * @return The value in the grid cell
         */
        core::ObjectDataPtr GetValue(int x, int y, int z = 0) const;

        /**
         * Gets the value in the grid cell at the given position.
         * @param x The x axis value
         * @param y The y axis value
         * @param z The z axis value
         * @return The value in the grid cell
         */
        core::ObjectDataPtr GetValue(double x, double y, double z = 0.0) const;

        /**
         * Gets the number of elements on the x axis.
         * @return The number of elements on the x axis
         */
        int GetWidthCount() const;

        /**
         * Gets the number of elements on the y axis.
         * @return The number of elements on the y axis
         */
        int GetHeightCount() const;

        /**
         * Gets the number of elements on the z axis.
         * @return The number of elements on the z axis
         */
        int GetDepthCount() const;

        /**
         * Gets the size of the whole grid on the x axis.
         * @return the size of the whole grid on the x axis
         */
        double GetWidth() const;

        /**
         * Gets the size of the whole grid on the y axis.
         * @return the size of the whole grid on the y axis
         */
        double GetHeight() const;

        /**
         * Gets the size of the whole grid on the z axis.
         * @return the size of the whole grid on the z axis
         */
        double GetDepth() const;

        /**
         * Converts a 3D position to an 3D index.
         * @param x The x axis value
         * @param y The y axis value
         * @param z The z axis value
         * @param xi The x axis index
         * @param yi The y axis index
         * @param zi The z axis index
         */
        void PositionToIndex(double x, double y, double z,
                             int& xi, int& yi, int& zi) const;

        //TODO comment
        void Convolve2D(int vicinity, double* mask, double multiplier);
        void Convolve3D(int vicinity, double* mask, double multiplier);
    };
}


#endif //GBMOT_GRID_H
