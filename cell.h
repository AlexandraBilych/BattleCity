# pragma once

#include <QObject>
#include "boardobject.h"
#include "enums.h"

class CellIsEmptyException {};

class Cell : public QObject
{
public:
    explicit Cell(int x, int y, QObject *parent = nullptr);
    explicit Cell(int x, int y, BoardObject* obj, QObject *parent = nullptr);
    ~Cell();

    BoardObject* getBoardObject();
    void setBoardObject(BoardObject* obj);

    bool isCellEmpty();
    void clearBoardObject();

//private:
    int x;
    int y;
    private:
    bool isEmpty;
    BoardObject* obj;
};
