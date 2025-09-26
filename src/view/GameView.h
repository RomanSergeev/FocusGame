#pragma once
#include <memory>
#include "Constants.h"
#include "model/GameModel.h"
#include "shapes/OpenGLShape.h"

class GameView {
public:
    enum class BoardShapeType {
        Flat,
        Toroid,
        Sphere
    };
private:
    struct CellView {
        std::unique_ptr<OpenGLShape> shape;
        glm::vec3 anchorPoint;
        glm::vec3 upVector;

        CellView() = default;
        CellView(const CellView& dc) = delete;
        CellView& operator = (const CellView& dc) = delete;
        CellView(CellView&& dc) noexcept = default;
        CellView& operator = (CellView&& dc) noexcept = default;
    };

    struct CheckerView {
        std::unique_ptr<OpenGLShape> shape;
        glm::vec3 position;
        glm::vec3 targetPosition;
        float animationProgress = 0.0f;
        const Checker* checkerRef;

        CheckerView() = default;
        CheckerView(const CheckerView& dc) = delete;
        CheckerView& operator = (const CheckerView& dc) = delete;
        CheckerView(CheckerView&& dc) noexcept = default;
        CheckerView& operator = (CheckerView&& dc) noexcept = default;
    };

    struct TrayView {
        std::unique_ptr<OpenGLShape> shape;
        glm::vec3 anchorPoint; // will switch to a vector of (center, upVector) for a pile of checkers
        glm::vec3 upVector;
        glm::vec3 position;
        const PlayerSlot ownedBy = PlayerSlot::Spectator;
        const PlayerSlot ofPlayer = PlayerSlot::Spectator;
        
        TrayView() = delete;
        TrayView(PlayerSlot ownedBy, PlayerSlot ofPlayer) : ownedBy(ownedBy), ofPlayer(ofPlayer) {}
        TrayView(const TrayView& dc) = delete;
        TrayView& operator = (const TrayView& dc) = delete;
        TrayView(TrayView&& dc) noexcept = default;
        TrayView& operator = (TrayView&& dc) noexcept = delete; // only move assignment
    };

    // static const int NO_SUCH_TRAY = -1;
    static constexpr float FB_CELL_WIDTH = 2; // flat board
    static constexpr float FB_CELL_HEIGHT_J = .6;
    static constexpr float FB_CELL_HEIGHT_NJ = 2;
    static constexpr float CHECKER_HALF_WIDTH = .8;
    static constexpr float CHECKER_HALF_HEIGHT = .1;
    static constexpr float TRAY_GAP = CHECKER_HALF_WIDTH * 2;
    static const float TRAY_DIMENSIONS[3];
    static const float FB_CELL_HEIGHT[2];
    static const float FB_CELL_ORIGIN[2];
    static const float FB_CELL_ANCHOR[2];

    static glm::vec3 calculateTrayPosition(const Coord& boardSizes, int totalTrays, int trayIndex);
    
    const GameModel& model;
    std::vector<std::vector<CellView>> displayedBoard;
    std::vector<CheckerView> displayedCheckers;
    std::map<PlayerSlot, std::vector<TrayView>> displayedTrays;
    std::unordered_map<PlayerSlot, Color> displayedPlayerColors;
    BoardShapeType type;

    // int getTrayIndex(PlayerSlot slot) const;
    void fillDisplayedBoard();
    CheckerView createCheckerView(const Checker& c, const glm::vec3& position);
    TrayView createTrayView(PlayerSlot owner, PlayerSlot ofPlayer, const glm::vec3& position);
    void createTrays();
public:
    GameView(const GameModel& gm, BoardShapeType boardShapeType);
    GameView(const GameView& gv) = delete;
    GameView& operator = (const GameView& gv) = delete;

    void draw(PlayerSlot perspective, const Shader& shader, float currentTime);
    void TEMPdeselectAll();
    void TEMPselectDistinctCell(int i, int j);
    void TEMPselectDistinctChecker(const Checker& c);
    void TEMPstageCheckerSelection(const Checker& c);
    float TEMPselectShapeByIntersection(const Ray& ray);
    void updatePlayerColors(const SessionKey& key, const std::unordered_map<PlayerSlot, Color>& colors) { displayedPlayerColors = colors; }
};