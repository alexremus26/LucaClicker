#include "Display.h"

#include <algorithm>
#include <iostream>

#include <SFML/Audio/SoundBuffer.hpp>

#include "ResourceManager.h"
#include "GameExceptions.h"
#include "Menu.h"

template <typename T>
void Display::centerOrigin(T& object) {
    auto bounds = object.getLocalBounds();
    object.setOrigin(bounds.getCenter());
}

template void Display::centerOrigin<sf::Text>(sf::Text&);
template void Display::centerOrigin<sf::Sprite>(sf::Sprite&);


sf::Texture & Display::getEmptyTexture() {
    static sf::Texture texture;
    if (texture.getSize().x == 0) {
        (void)texture.resize({1, 1});
    }
    return texture;
}

Display::Display(Game& manager)
    : gameManager(manager)
{
}

Display::~Display() = default;

std::ostream& operator<<(std::ostream& os, const Display& display)
{
    os << display.gameManager;
    return os;
}

sf::Vector2f Display::rectCenter(const sf::FloatRect& r)
{
    return {r.position.x + r.size.x * 0.5f, r.position.y + r.size.y * 0.5f};
}

void Display::loadFont()
{
    font = ResourceManager<sf::Font>::instance().get("assets/font/MightySouly-lxggD.ttf");
}

void Display::createMenuWindow()
{
    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(sf::VideoMode(MenuSize, desktop.bitsPerPixel), "Luca Clicker", sf::Style::Titlebar | sf::Style::Close);
}

void Display::createGameWindow()
{
    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(sf::VideoMode(desktop), "Luca Clicker", sf::Style::Default);
    window.setView(window.getDefaultView());
}

sf::Text Display::makeCenteredText(const std::string& str, unsigned int size, const sf::Color& color, const sf::Vector2f& center)
{
    sf::Text t(font, str, size);
    t.setFillColor(color);
    Display::centerOrigin(t);
    t.setPosition(center);
    return t;
}

sf::Text Display::makeSmallLabel(const std::string& str, const sf::FloatRect& bounds, const sf::Color& color) const {
    sf::Text t(font, str);
    t.setFillColor(color);
    t.setCharacterSize(static_cast<unsigned int>(bounds.size.y * 0.22f));
    t.setPosition({
        bounds.position.x + bounds.size.x * 0.1f,
        bounds.position.y + bounds.size.y * 0.12f
    });
    return t;
}

sf::Text Display::makeValueLabel(const std::string& str, const sf::FloatRect& bounds, const sf::Color& color)
{
    sf::Text t(font, str);
    t.setFillColor(color);
    t.setCharacterSize(static_cast<unsigned int>(bounds.size.y * 0.25f));
    Display::centerOrigin(t); 
    t.setPosition(rectCenter(bounds));
    t.move({0.f, 20.f});
    return t;
}

void Display::drawProgressBar(const float progress, const float x, const float y, const float scale)
{
    const float clamped = std::clamp(progress, 0.f, 1.f);

    static const sf::Texture& outlineTex =
        ResourceManager<sf::Texture>::instance().get("assets/textures/ProgressBarOutline.png");

    static const sf::Texture& fillTex =
        ResourceManager<sf::Texture>::instance().get("assets/textures/ProgressBarFull.png");

    sf::Sprite outline(outlineTex);
    sf::Sprite fill(fillTex);

    outline.setScale({scale, scale});
    fill.setScale({scale, scale});

    outline.setPosition({x, y});
    fill.setPosition({x, y});

    const sf::Vector2u size = fillTex.getSize();
    const auto clippedWidth = static_cast<unsigned int>(static_cast<float>(size.x) * clamped);

    fill.setTextureRect({
        {0, -5},
        {static_cast<int>(clippedWidth), static_cast<int>(size.y)}
    });

    window.draw(outline);
    window.draw(fill);
}

void Display::run()
{
    createMenuWindow();

    Menu menu(window);
    menu.show();

    if (menu.wantsExit())
        return;

    loadFont();

    if (menu.wantsLoadGame()) {
        try {
            if (!gameManager.loadSavedGame())
                std::cerr << "Warning: save file could not be loaded.\n";
        } catch (const SaveStateException& e) {
            std::cerr << "An error occurred while loading saved game: " << e.what() << '\n';
        }
    }

    createGameWindow();
    initGame();
    gameLoop();
}

void Display::initGame()
{
    ui.items.clear();
    ui.beverages.clear();

    computeLayout();
    initAudio();
    initMoneyText();
    buildRows();

    ui.tooltip.background.setFillColor(sf::Color(0, 0, 0, 190));
    ui.tooltip.background.setOutlineColor(sf::Color(200, 200, 200));
    ui.tooltip.background.setOutlineThickness(1.f);
    ui.tooltip.text.emplace(font, "", 24);
    ui.tooltip.text->setFillColor(sf::Color::White);
    ui.tooltip.text->setOutlineColor(sf::Color::Black);
    ui.tooltip.text->setOutlineThickness(1.f);
}

void Display::computeLayout()
{
    const float winW = static_cast<float>(window.getSize().x);
    const float winH = static_cast<float>(window.getSize().y);

    heightScale = winH / ReferenceHeight;

    baseScale = heightScale * 0.3f;
    buyScale = baseScale * 1.25f;
    timeScale = baseScale * 0.85f;

    progressScale = baseScale * 2.0f;

    const float aspectRatio = winW / winH;
    const float leftMargin = (aspectRatio >= 1.7f) ? 0.03f : 0.04f;

    leftX = winW * leftMargin;
    startY = winH * 0.18f;
    spacingY = winH * 0.155f;

    const float horizontalSpacing = winW * 0.015f;
    const float columnGap = winW * (aspectRatio >= 2.0f ? 0.06f : 0.08f);

    const sf::Texture& holderTex = ResourceManager<sf::Texture>::instance().get("assets/textures/ItemHolder.png");
    const sf::Texture& buyTex = ResourceManager<sf::Texture>::instance().get("assets/textures/BuyButton.png");
    const sf::Texture& timeTex = ResourceManager<sf::Texture>::instance().get("assets/textures/TimeIntervalButton.png");

    progressX = leftX + static_cast<float>(holderTex.getSize().x) * baseScale + horizontalSpacing;
    buyX = progressX;
    timeX = buyX + static_cast<float>(buyTex.getSize().x) * buyScale + horizontalSpacing * 0.67f;

    secondColumnX = timeX + static_cast<float>(timeTex.getSize().x) * timeScale + columnGap;
    useButtonX = secondColumnX + static_cast<float>(holderTex.getSize().x) * baseScale + horizontalSpacing;
}

void Display::initAudio()
{
    if (audio.started)
        return;

    const auto& buf = ResourceManager<sf::SoundBuffer>::instance().get("assets/audio/adventure_capitalist_theme_song.wav");
    audio.soundtrack.emplace(buf);
    audio.soundtrack->setLooping(true);
    audio.soundtrack->setVolume(50.f);
    audio.soundtrack->play();
    audio.started = true;
}

void Display::initMoneyText()
{
    const float winW = static_cast<float>(window.getSize().x);
    const float winH = static_cast<float>(window.getSize().y);

    ui.moneyText.emplace(font, "", static_cast<unsigned int>(90.f * heightScale));
    ui.moneyText->setOutlineThickness(3.f);
    ui.moneyText->setOutlineColor(sf::Color::Black);
    ui.moneyText->setFillColor(sf::Color(255, 220, 120));
    ui.moneyText->setPosition({winW * 0.03f, winH * 0.03f});
}

void Display::buildRows()
{
    ui.items.reserve(ItemCount);
    ui.beverages.reserve(ItemCount);

    for (int i = 0; i < ItemCount; ++i)
        ui.items.push_back(buildItemRow(i));

    for (int i = 0; i < ItemCount; ++i)
        ui.beverages.push_back(buildBeverageRow(i));
}

Display::RowUI Display::buildItemRow(int index)
{
    const sf::Texture& holderTex = ResourceManager<sf::Texture>::instance().get("assets/textures/ItemHolder.png");
    const sf::Texture& buyTex = ResourceManager<sf::Texture>::instance().get("assets/textures/BuyButton.png");
    const sf::Texture& timeTex = ResourceManager<sf::Texture>::instance().get("assets/textures/TimeIntervalButton.png");

    const std::vector<std::string> icons = {
        "assets/textures/Pretzel.png",
        "assets/textures/Pizza.png",
        "assets/textures/ApplePie.png",
        "assets/textures/Strudel.png",
        "assets/textures/CovriLuca.png"
    };

    const float winH = static_cast<float>(window.getSize().y);
    const float y = startY + static_cast<float>(index) * spacingY;

    sf::Sprite holder(holderTex);
    holder.setScale({baseScale, baseScale});
    holder.setPosition({leftX, y});

    sf::Sprite icon(ResourceManager<sf::Texture>::instance().get(icons[index]));
    const float holderHeight = static_cast<float>(holderTex.getSize().y) * baseScale;
    const float iconScale = holderHeight * 0.5f / static_cast<float>(icon.getTexture().getSize().y);
    icon.setScale({iconScale, iconScale});
    
    Display::centerOrigin(icon);
    
    icon.setPosition(rectCenter(holder.getGlobalBounds()));
    icon.move({0.f, -20.f});

    const auto& itemPtr = gameManager.getItems()[index];
    const sf::FloatRect hb = holder.getGlobalBounds();

    sf::Text name = makeCenteredText(itemPtr->getName(), static_cast<unsigned int>(22.f * heightScale), sf::Color(60, 60, 60),
                                    {rectCenter(hb).x, hb.position.y + hb.size.y * 0.78f});

    sf::Text level = makeCenteredText("Level " + std::to_string(itemPtr->getLevel()), static_cast<unsigned int>(18.f * heightScale), sf::Color(80, 80, 80),
                                     {rectCenter(hb).x, hb.position.y + hb.size.y * 0.92f});

    sf::Sprite buyBtn(buyTex);
    buyBtn.setScale({buyScale, buyScale});
    buyBtn.setPosition({buyX, y + winH * 0.055f});

    sf::Sprite upgBtn(timeTex);
    upgBtn.setScale({timeScale, timeScale});
    upgBtn.setPosition({timeX, y + winH * 0.062f});

    const sf::FloatRect bb = buyBtn.getGlobalBounds();
    const sf::FloatRect ub = upgBtn.getGlobalBounds();

    sf::Text buyLabel = makeSmallLabel("SELL", bb, sf::Color::Black);
    sf::Text buyValue = makeValueLabel("0", bb, sf::Color::Black);

    sf::Text upgLabel = makeSmallLabel("UPGRADE", ub, sf::Color::Black);
    sf::Text upgValue = makeValueLabel("0", ub, sf::Color::White);

    return RowUI{
        holder,
        icon,
        buyBtn,
        upgBtn,
        name,
        level,
        buyLabel,
        buyValue,
        upgLabel,
        upgValue,
        false,
        false,
        false
    };
}

Display::RowUI Display::buildBeverageRow(int index)
{
    const sf::Texture& holderTex = ResourceManager<sf::Texture>::instance().get("assets/textures/ItemHolder.png");
    const sf::Texture& buyTex = ResourceManager<sf::Texture>::instance().get("assets/textures/BuyButton.png");

    const std::vector<std::string> icons = {
        "assets/textures/Water.png",
        "assets/textures/Coke.png",
        "assets/textures/Coffee.png",
        "assets/textures/Ayran.png",
        "assets/textures/Matcha.png"
    };

    const float winH = static_cast<float>(window.getSize().y);
    const float y = startY + static_cast<float>(index) * spacingY;

    sf::Sprite holder(holderTex);
    holder.setScale({baseScale, baseScale});
    holder.setPosition({secondColumnX, y});

    sf::Sprite icon(ResourceManager<sf::Texture>::instance().get(icons[index]));
    const float holderHeight = static_cast<float>(holderTex.getSize().y) * baseScale;
    const float iconScale = holderHeight * 0.45f / static_cast<float>(icon.getTexture().getSize().y);
    icon.setScale({iconScale, iconScale});
    
    Display::centerOrigin(icon);
    
    icon.setPosition(rectCenter(holder.getGlobalBounds()));
    icon.move({0.f, -22.f});

    const std::size_t bevIndex = static_cast<std::size_t>(index) + 5u;
    const auto& bevPtr = gameManager.getItems()[bevIndex];
    const sf::FloatRect hb = holder.getGlobalBounds();

    sf::Text name = makeCenteredText(bevPtr->getName(), static_cast<unsigned int>(22.f * heightScale), sf::Color(60, 60, 60),
                                    {rectCenter(hb).x, hb.position.y + hb.size.y * 0.78f});

    sf::Text level = makeCenteredText("Level " + std::to_string(bevPtr->getLevel()), static_cast<unsigned int>(18.f * heightScale), sf::Color(80, 80, 80),
                                     {rectCenter(hb).x, hb.position.y + hb.size.y * 0.92f});

    sf::Sprite useBtn(buyTex);
    useBtn.setScale({buyScale, buyScale});
    useBtn.setPosition({useButtonX, y + winH * 0.055f});

    const sf::FloatRect ub = useBtn.getGlobalBounds();

    sf::Text useLabel = makeSmallLabel("USE", ub, sf::Color::Black);
    sf::Text useValue = makeValueLabel("0", ub, sf::Color::Black);

    sf::Sprite empty(getEmptyTexture());

    sf::Text emptyT(font, "", 1);

    return RowUI{
        holder,
        icon,
        useBtn,
        empty,
        name,
        level,
        useLabel,
        useValue,
        emptyT,
        emptyT,
        false,
        false,
        false
    };
}

void Display::gameLoop()
{
    while (window.isOpen()) {
        pollEvents();
        if (!window.isOpen())
            break;
        updateFrame();
        renderFrame();
    }
}

void Display::pollEvents()
{
    while (const auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            gameManager.saveGame();
            window.close();
            return;
        }

        if (const auto* mbp = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mbp->button == sf::Mouse::Button::Left) {
                const sf::Vector2f mouse = window.mapPixelToCoords(mbp->position);
                onClick(mouse);
            }
        }
    }
}

void Display::onClick(const sf::Vector2f& mouse) const
{
    for (int i = 0; i < ItemCount; ++i)
        handleItemClick(i, mouse);

    for (int i = 0; i < ItemCount; ++i)
        handleBeverageClick(i, mouse);
}

void Display::handleItemClick(const int index, const sf::Vector2f& mouse) const {
    auto& row = ui.items[index];

    if (row.primaryButton.getGlobalBounds().contains(mouse)) {
        if (!gameManager.isUnlocked(index)) {
            gameManager.unlockItem(index);
        } else if (!gameManager.isSelling(index)) {
            const auto& item = *gameManager.getItems()[index];
            gameManager.runSellingLoop(item, index);
        }
    }

    if (&row.secondaryButton.getTexture() != &getEmptyTexture() && row.secondaryButton.getGlobalBounds().contains(mouse)) {
        if (gameManager.isUnlocked(index)) {
            auto& item = *gameManager.getItems()[index];
            gameManager.upgrade(item);
        }
    }
}

void Display::handleBeverageClick(const int index, const sf::Vector2f& mouse) const {
    const auto& row = ui.beverages[index];

    if (!row.primaryButton.getGlobalBounds().contains(mouse))
        return;

    const std::size_t bevIndex = static_cast<std::size_t>(index) + 5u;

    if (!gameManager.isUnlocked(bevIndex)) {
        gameManager.unlockItem(bevIndex);
    } else {
        gameManager.useItem(bevIndex);
    }
}

void Display::updateFrame()
{
    gameManager.updateSelling();

    const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    updateHover(mouse);
    updateMoney();
    updateRowsText();
    updateTooltip(mouse);
}

void Display::updateHover(const sf::Vector2f& mouse)
{
    for (int i = 0; i < ItemCount; ++i) {
        ui.items[i].primaryHovered = ui.items[i].primaryButton.getGlobalBounds().contains(mouse);
        ui.items[i].secondaryHovered = &ui.items[i].secondaryButton.getTexture() != &getEmptyTexture() && ui.items[i].secondaryButton.getGlobalBounds().contains(mouse);
        ui.items[i].holderHovered = ui.items[i].holder.getGlobalBounds().contains(mouse);

        ui.beverages[i].primaryHovered = ui.beverages[i].primaryButton.getGlobalBounds().contains(mouse);
        ui.beverages[i].holderHovered = ui.beverages[i].holder.getGlobalBounds().contains(mouse);
    }
}

void Display::updateMoney()
{
    const double money = gameManager.getPlayerMoney();
    if (ui.moneyText)
        ui.moneyText->setString("$ " + std::to_string(static_cast<long long>(money)));
}

void Display::updateRowsText()
{
    for (int i = 0; i < ItemCount; ++i) {
        const Item& item = *gameManager.getItems()[i];

        ui.items[i].level.setString("Level " + std::to_string(item.getLevel()));
        Display::centerOrigin(ui.items[i].level);

        const bool unlocked = gameManager.isUnlocked(i);

        ui.items[i].primaryLabel.setString(unlocked ? "SELL" : "UNLOCK");
        const double buyValue = unlocked ? item.sellPayout() : item.getUnlockCost();
        ui.items[i].primaryValue.setString(std::to_string(static_cast<long long>(buyValue)));

        if (unlocked) {
            ui.items[i].secondaryLabel.setString("UPGRADE");
            const double upg = item.getUpgradeCost();
            ui.items[i].secondaryValue.setString(upg > 0 ? std::to_string(static_cast<long long>(upg)) : "-");
        } else {
            ui.items[i].secondaryLabel.setString("");
            ui.items[i].secondaryValue.setString("");
        }

        const std::size_t bevIndex = static_cast<std::size_t>(i) + 5u;
        const Item& bev = *gameManager.getItems()[bevIndex];

        ui.beverages[i].level.setString("Level " + std::to_string(bev.getLevel()));
        Display::centerOrigin(ui.beverages[i].level);


        if (!gameManager.isUnlocked(bevIndex)) {
            ui.beverages[i].primaryLabel.setString("UNLOCK");
            ui.beverages[i].primaryValue.setString(std::to_string(static_cast<long long>(bev.getUnlockCost())));
        } else if (!bev.isUsable()) {
            ui.beverages[i].primaryLabel.setString("");
            ui.beverages[i].primaryValue.setString("");
        } else {
            ui.beverages[i].primaryLabel.setString("USE");
            ui.beverages[i].primaryValue.setString(std::to_string(static_cast<long long>(bev.getUseCost())));
        }
    }
}

std::string Display::tooltipContent() const
{
    for (int i = 0; i < ItemCount; ++i) {
        if (ui.items[i].holderHovered)
            return gameManager.getItemSpecialty(i);
        if (ui.beverages[i].holderHovered)
            return gameManager.getItemSpecialty(i + 5);
    }
    return {};
}

void Display::updateTooltip(const sf::Vector2f& mouse)
{
    ui.tooltip.content = tooltipContent();
    ui.tooltip.visible = !ui.tooltip.content.empty();
    if (!ui.tooltip.visible)
        return;

    if (!ui.tooltip.text)
        return;

    ui.tooltip.text->setString(ui.tooltip.content);

    const auto tb = ui.tooltip.text->getLocalBounds();
    ui.tooltip.background.setSize({tb.size.x + 20.f, tb.size.y + 20.f});

    sf::Vector2f pos = mouse + sf::Vector2f(20.f, 20.f);
    const auto winSize = sf::Vector2f(window.getSize());

    if (pos.x + ui.tooltip.background.getSize().x > winSize.x)
        pos.x = winSize.x - ui.tooltip.background.getSize().x;
    if (pos.y + ui.tooltip.background.getSize().y > winSize.y)
        pos.y = winSize.y - ui.tooltip.background.getSize().y;

    ui.tooltip.background.setPosition(pos);
    ui.tooltip.text->setPosition(pos + sf::Vector2f(10.f, 10.f));
}

void Display::renderFrame()
{
    window.clear();
    drawBackground();
    drawMoney();
    drawItems();
    drawBeverages();
    drawTooltip();
    window.display();
}

void Display::drawBackground()
{
    sf::Sprite bg(ResourceManager<sf::Texture>::instance().get("assets/textures/GameBackground.png"));
    bg.setScale({
        static_cast<float>(window.getSize().x) / static_cast<float>(bg.getTexture().getSize().x),
        static_cast<float>(window.getSize().y) / static_cast<float>(bg.getTexture().getSize().y)
    });
    window.draw(bg);
}

void Display::drawMoney()
{
    if (ui.moneyText)
        window.draw(*ui.moneyText);
}

void Display::drawItems()
{
    for (int i = 0; i < ItemCount; ++i) {
        auto& row = ui.items[i];

        window.draw(row.holder);
        window.draw(row.name);
        window.draw(row.level);
        window.draw(row.icon);

        sf::Color buyColor = row.primaryHovered ? sf::Color(255, 230, 160) : sf::Color::White;
        if (gameManager.isSelling(i))
            buyColor = sf::Color(180, 180, 180);

        row.primaryButton.setColor(buyColor);
        row.secondaryButton.setColor(row.secondaryHovered ? sf::Color(200, 200, 255) : sf::Color::White);

        window.draw(row.primaryButton);
        window.draw(row.primaryLabel);
        window.draw(row.primaryValue);

        if (&row.secondaryButton.getTexture() != &getEmptyTexture()) {
            window.draw(row.secondaryButton);
            window.draw(row.secondaryLabel);
            window.draw(row.secondaryValue);
        }

        drawProgressBar(gameManager.anyProgress(i), progressX, startY + static_cast<float>(i) * spacingY, progressScale);
    }
}

void Display::drawBeverages()
{
    for (int i = 0; i < ItemCount; ++i) {
        auto& row = ui.beverages[i];

        window.draw(row.holder);
        window.draw(row.name);
        window.draw(row.level);
        window.draw(row.icon);

        row.primaryButton.setColor(row.primaryHovered ? sf::Color(160, 230, 255) : sf::Color::White);

        window.draw(row.primaryButton);
        window.draw(row.primaryLabel);
        window.draw(row.primaryValue);
    }
}

void Display::drawTooltip()
{
    if (!ui.tooltip.visible)
        return;

    window.draw(ui.tooltip.background);
    if (ui.tooltip.text)
        window.draw(*ui.tooltip.text);
}