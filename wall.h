# pragma once

#include "boardobject.h"

class Wall : public BoardObject
{ 
    Q_OBJECT

public:
    explicit Wall(int direction, int coordinate_x, int coordinate_y, QObject *parent = nullptr);
    virtual ~Wall();

    float getHitPoints() const;

    void setHitPoints(float value);

    virtual void move() {}

private:
    float hitPoints;
};
