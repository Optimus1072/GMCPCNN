//
// Created by wrede on 11.07.16.
//

#ifndef GBMOT_FILTER_H
#define GBMOT_FILTER_H


#include <vector>
#include <string>

namespace util
{
    /**
     * Utility class for a two-dimensional filter.
     */
    class Filter2D
    {
    public:
        /**
         * Creates a new instance with an identity filter.
         */
        Filter2D();

        /**
         * Creates a new instance with the given multiplier and mask values.
         * To calculate the value all values are added with their weights from the mask and then
         * the value is multiplied by the multiplier. The multiplier may be used to ensure the
         * overall filter sum is equals one.
         *
         * @param multiplier The multiplier
         * @param mask The mask values
         */
        Filter2D(double multiplier, std::vector<double> mask);

        /**
         * Creates a new instance by parsing the specified mask string.
         * Values are separated by the specified delimiter.
         * Example: 'multiplier, m00, m01, m02, ..., m10, m11, m12, ..., mnn' as the mask string
         * and ',' as the delimiter.
         *
         * @param mask_string The string with the multiplier and the mask values to parse
         * @param delimiter The delimiter used for the parsing
         */
        Filter2D(std::string const & mask_string, char delimiter);

        /**
         * Gets the value in the mask at the specified position.
         *
         * @param x The horizontal position
         * @param y The vertical position
         * @return The value
         */
        double Get(int x, int y) const;

        /**
         * Gets the value in the mask at the specified position.
         *
         * @param i The index (row-major)
         * @return The value
         */
        double Get(int i) const;

        /**
         * Gets the multiplier.
         *
         * @return The multiplier
         */
        double GetMultiplier() const;

        /**
         * Gets the vicinity of the mask.
         * The vicinity is the based on the center and describes the maximum range of values
         * around it in manhattan distance.
         *
         * @return The vicinity
         */
        int GetVicinity() const;

        /**
         * Gets the dimension of the mask.
         *
         * @return The dimension
         */
        int GetDimension() const;

        /**
         * Normalizes the mask values.
         * The multiplier is equals one afterwards.
         */
        void Normalize();

    private:
        /**
         * Calculates the index for the specified x and y values.
         * The mask is stored in row-major order.
         *
         * @param x The horizontal value
         * @param y The vertical value
         * @return The index
         */
        int Index(int x, int y) const;

        /**
         * Calculates the vicinity for the specified size.
         * The vicinity is the based on the center and describes the maximum range of values
         * around it in manhattan distance.
         *
         * @param size The size
         * @return The vicinity
         */
        int Vicinity(int size) const;

        /**
         * Calculates the dimension for the specified size.
         *
         * @param size The size
         * @return The dimension
         */
        int Dimension(int size) const;

        /**
         * The mask values stored in row-major order
         */
        std::vector<double> mask_;

        /**
         * The multiplier
         */
        double multiplier_;

        /**
         * The count of the values in the mask
         */
        int dimension_;

        /**
         * The vicinity is the based on the center and describes the maximum range of values
         * around it in manhattan distance
         */
        int vicinity_;
    };
}


#endif //GBMOT_FILTER_H
