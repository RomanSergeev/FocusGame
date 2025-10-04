#pragma once
#include <memory>
#include "Constants.h"
#include "graphics/RenderEnums.h"
#include "model/Checker.h"
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
        const Cell* cellRef;

        CellView() = default;
        CellView(const CellView& dc) = delete;
        CellView& operator = (const CellView& dc) = delete;
        CellView(CellView&& dc) noexcept = default;
        CellView& operator = (CellView&& dc) noexcept = default;

        void select(SelectionType type);
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

        void select(SelectionType type);
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

    static constexpr float FB_CELL_WIDTH = 2; // flat board
    static constexpr float FB_CELL_HEIGHT_J = .6;
    static constexpr float FB_CELL_HEIGHT_NJ = 2;
    static constexpr float CHECKER_HALF_WIDTH = .8;
    static constexpr float CHECKER_HALF_HEIGHT = .1;
    static constexpr float TRAY_GAP = CHECKER_HALF_WIDTH * 2;
    static const float TRAY_DIMENSIONS[3];
    static const float FB_CELL_HEIGHT[2];
    static const float FB_CELL_ORIGIN[2];

    static glm::vec3 calculateTrayPosition(const Coord& boardSizes, int totalTrays, int trayIndex);
    static CellView createCellView(const Cell& c, const glm::vec3& position, const glm::vec3& dimensions, const Color& color);
    static CheckerView createCheckerView(const Checker& c, const glm::vec3& position);
    static TrayView createTrayView(PlayerSlot owner, PlayerSlot ofPlayer, const glm::vec3& position);
    
    const GameModel& model;
    std::vector<std::vector<CellView>> displayedBoard;
    std::vector<CheckerView> displayedCheckers;
    std::map<PlayerSlot, std::vector<TrayView>> displayedTrays;
    std::unordered_map<PlayerSlot, Color> displayedPlayerColors;
    BoardShapeType type;

    void createDisplayedBoard();
    void createTrays();
public:
    // view switch: either a CellView or a CheckerView can be selected
    // changed from const reference to reference: SelectedView is used to select shapes, which is not const operation
    struct SelectedView {
        SelectedView() = default;
        SelectedView(CellView& c) { setCellView(c); }
        SelectedView(CheckerView& c) { setCheckerView(c); }

        SelectedView(const SelectedView& sv) : cellPtr(sv.cellPtr), checkerPtr(sv.checkerPtr) {}

        bool operator == (const SelectedView& sv) const { return cellPtr == sv.cellPtr && checkerPtr == sv.checkerPtr; }
        bool operator != (const SelectedView& sv) const { return !(*this == sv); }

        bool isEmpty() const { return !isCellView() && !isCheckerView(); }
        bool isCellView() const { return cellPtr != nullptr; }
        bool isCheckerView() const { return checkerPtr != nullptr; }
        void setCellView(CellView& c) { cellPtr = &c; checkerPtr = nullptr; }
        void setCheckerView(CheckerView& c) { checkerPtr = &c; cellPtr = nullptr; }
        void drop() { cellPtr = nullptr; checkerPtr = nullptr; }
        OpenGLShape* getShapePtr() { return cellPtr ? cellPtr->shape.get() : checkerPtr ? checkerPtr->shape.get() : nullptr; }
        const OpenGLShape* getShapePtr() const { return cellPtr ? cellPtr->shape.get() : checkerPtr ? checkerPtr->shape.get() : nullptr; }
        void select(SelectionType type);
    private:
        CellView* cellPtr = nullptr;
        CheckerView* checkerPtr = nullptr;
    };
    GameView(const GameModel& gm, BoardShapeType boardShapeType);
    GameView(const GameView& gv) = delete;
    GameView& operator = (const GameView& gv) = delete;

    void draw(PlayerSlot perspective, const Shader& shader, float currentTime);
    SelectedView getHoveredShape(const SessionKey& key, const Ray& ray);
    void updatePlayerColors(const SessionKey& key, const std::unordered_map<PlayerSlot, Color>& colors) { displayedPlayerColors = colors; }
};