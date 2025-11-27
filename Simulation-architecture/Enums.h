#pragma once

enum class CellType { EMPTY, WALL, START, END, SPECIAL };
enum class GameState { IDLE, SOLVING, COMPLETE, FAILED, PAUSED };
enum class RobotState { IDLE, CALCULATING, MOVING, COMPLETED };
enum class DifficultyLevel { EASY, MEDIUM, HARD, EXPERT };
