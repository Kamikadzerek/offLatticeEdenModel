#ifndef OFFLATICEEDENMODEL_CELLPRIMITIVE_H
#define OFFLATICEEDENMODEL_CELLPRIMITIVE_H


class CellPrimitive
{
public:
    CellPrimitive(double X, double Y)
    {
        extern const double SIZE;
        x = X;
        y = Y;
        size = SIZE;
    }
    double getX() const
    {
        return x;
    }
    double getY() const
    {
        return y;
    }
    double getSize() const
    {
        return size;
    }

private:
    double x;
    double y;
    double size;
};


#endif//OFFLATICEEDENMODEL_CELLPRIMITIVE_H
