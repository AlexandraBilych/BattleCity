#include "board.h"
#include <QGuiApplication>
#include <QScreen>
#include <QWindow>

const int step = 10;

Board::Board(QObject *parent) : QObject(parent)
{
    qDebug() << "Constructor Board";

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    int height = screenGeometry.height();
    int width = screenGeometry.width();

    // divide the board into cells
    int rowsAmount = height / step;
    if (height % step != 0)
        rowsAmount += 1;

    int columnsAmount = width / step;
    if (width % step != 0)
        columnsAmount += 1;

    qDebug() << "rowsAmount - " << rowsAmount << ", columnsAmount - " << columnsAmount;

    cells.resize(rowsAmount);
    for (int row = 0; row < rowsAmount; row++) {
        for(int col = 0; col < columnsAmount; col++) {
            cells[row].append(new Cell(row * step, col * step));
        }
    }

    //create enemy tanks
    int enemiesAmount = 6; // TO DO: move to commmon config
    int distance = width/enemiesAmount;

    for (int i = 0; i < enemiesAmount; i++) {
        Enemy* enemy = new Enemy(100, 20, distance * i + distance / 2, 0);
        m_enemies.append(enemy);

        int firstRow = 0;
        int firstCol = (distance * i + distance / 2) / step;
        rowsAmount = enemy->getHeight() / step;
        columnsAmount = enemy->getWidth() / step;

        for (int row = 0; row < rowsAmount; row++) {
            for(int col = 0; col < columnsAmount; col++) {
                qDebug() << "Add enemy tank to cell: " << (firstRow + row) << ", " << (firstCol + col);
                cells[firstRow + row][firstCol + col]->setBoardObject(enemy);
            }
        }
    }

    //create player's tank
    int playerX = width / 2 - 50;
    int playerY = height - 100;
    player = new Player(100, 20, "PlayerTank", playerX, playerY);

    int firstRow = playerY / step;
    int firstCol = playerX / step;
    rowsAmount = player->getHeight() / step;
    columnsAmount = player->getWidth() / step;

    for (int row = 0; row < rowsAmount; row++) {
        for(int col = 0; col < columnsAmount; col++) {
            qDebug() << "Add player tank to cell: " << (firstRow + row) << ", " << (firstCol + col);
            cells[firstRow + row][firstCol + col]->setBoardObject(player);
        }
    }
}

Board::~Board()
{
    for (auto enemie : m_enemies) {
        delete enemie;
    }

    m_enemies.clear();
}

bool Board::AreCellsFree(QVector<Cell*> cells)
{
    // check if cells are avaliable
    bool areFree = true;

    for (auto cell : cells) {
        if (!cell->isCellEmpty()) {
            areFree = false;
            break;
        }
    }

    return areFree;
}

void Board::FreeCells(QVector<Cell*> cells)
{
    for (auto cell : cells)
        cell->clearBoardObject();
}

void Board::move(Tank *tank)
{
    // check if a cell is avaliable
    // if it's not than don't move tank
    QVector<Cell*> prevCells, nextCells;

    int firstRow = tank->getY() / step;
    int firstCol = tank->getX() / step;
    int direction = tank->getDirection();

    int nextIndex = 0;
    int prevIndex = 0;

    //calculate indexes for next and previous cells
    if ( direction == Direction::dir_down) {
        nextIndex = firstRow + tank->getHeight() / step;
        if (nextIndex * step >= qApp->focusWindow()->height()) {
            tank->updateDirection();
            return;
        }

        prevIndex = firstRow;

    } else if (direction == Direction::dir_up) {
        nextIndex = firstRow - 1;
        if (nextIndex * step <= 0) {
            tank->updateDirection();
            return;
        }

        prevIndex = firstRow + tank->getHeight() / step - 1;

    } else if (direction == Direction::dir_left) {
        nextIndex = firstCol - 1;
        if (nextIndex * step <= 0) {
            tank->updateDirection();
            return;
        }

        prevIndex = firstCol + tank->getWidth() / step - 1;
        qDebug() << "Left dir: nextIndex - " << nextIndex << ", prevIndex - " << prevIndex;

    } else if (direction == Direction::dir_right) {
        nextIndex = firstCol + tank->getWidth() / step;
        if (nextIndex * step >= qApp->focusWindow()->width()) {
            tank->updateDirection();
            return;
        }

        prevIndex = firstCol;
    }

    qDebug() << "INDEXES: nextIndex - " << nextIndex << ", prevIndex - " << prevIndex << ", dir - " << direction;
    if ( direction == Direction::dir_down || direction == Direction::dir_up) {
        int cellAmount = tank->getWidth() / step;

        for (int cell = 0; cell < cellAmount; cell++) {
            nextCells.append(cells[nextIndex][firstCol + cell]);
            prevCells.append(cells[prevIndex][firstCol + cell]);
        }
    } else {
        int cellAmount = tank->getHeight() / step;

        for (int cell = 0; cell < cellAmount; cell++) {
            nextCells.append(cells[firstRow + cell][nextIndex]);
            prevCells.append(cells[firstRow + cell][prevIndex]);
        }
    }

    if (AreCellsFree(nextCells)) {
        tank->move();
        for (auto cell : nextCells)
            cell->setBoardObject(tank);
        FreeCells(prevCells);
    } else {
        tank->updateDirection();
    }
}

QQmlListProperty<Enemy> Board::enemies()
{
    return QQmlListProperty<Enemy>(this, m_enemies);
}

QList<Enemy*> Board::getEnemies()
{
    return m_enemies;
}

Player* Board::getPlayer() const
{
    return player;
}

