# Generated from imageparticle.pro.

cmake_minimum_required(VERSION 3.14)
project(imageparticle LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/quick/particles/imageparticle")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS Qml)

qt_add_executable(imageparticle
    main.cpp
)
set_target_properties(imageparticle PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(imageparticle PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)


# Resources:
set(imageparticle_resource_files
    "content/allatonce.qml"
    "content/colored.qml"
    "content/colortable.qml"
    "content/deformation.qml"
    "content/rotation.qml"
    "content/sharing.qml"
    "content/sprites.qml"
    "imageparticle.qml"
)

qt6_add_resources(imageparticle "imageparticle"
    PREFIX
        "/particles/imageparticle"
    FILES
        ${imageparticle_resource_files}
)
set(images_resource_files
    "../images/_explo.png"
    "../images/backgroundLeaves.jpg"
    "../images/bear_tiles.png"
    "../images/candle.png"
    "../images/colortable.png"
    "../images/finalfrontier.png"
    "../images/flower.png"
    "../images/matchmask.png"
    "../images/meteor.png"
    "../images/meteor_explo.png"
    "../images/meteors.png"
    "../images/nullRock.png"
    "../images/particle2.png"
    "../images/particle3.png"
    "../images/particleA.png"
    "../images/portal_bg.png"
    "../images/realLeaf1.png"
    "../images/realLeaf2.png"
    "../images/realLeaf3.png"
    "../images/realLeaf4.png"
    "../images/rocket.png"
    "../images/rocket2.png"
    "../images/sizeInOut.png"
    "../images/snowflake.png"
    "../images/sparkleSize.png"
    "../images/starfish_0.png"
    "../images/starfish_1.png"
    "../images/starfish_2.png"
    "../images/starfish_3.png"
    "../images/starfish_4.png"
    "../images/starfish_mask.png"
)

qt6_add_resources(imageparticle "images"
    PREFIX
        "/particles"
    BASE
        ".."
    FILES
        ${images_resource_files}
)
set(shared_resource_files
    "../../shared/Button.qml"
    "../../shared/CheckBox.qml"
    "../../shared/FlickrRssModel.qml"
    "../../shared/Label.qml"
    "../../shared/LauncherList.qml"
    "../../shared/SimpleLauncherDelegate.qml"
    "../../shared/Slider.qml"
    "../../shared/TabSet.qml"
    "../../shared/TextField.qml"
    "../../shared/images/back.png"
    "../../shared/images/checkmark.png"
    "../../shared/images/next.png"
    "../../shared/images/qt-logo.png"
    "../../shared/images/slider_handle.png"
    "../../shared/images/tab.png"
)

qt6_add_resources(imageparticle "shared"
    PREFIX
        "/shared"
    BASE
        "../../shared"
    FILES
        ${shared_resource_files}
)

install(TARGETS imageparticle
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
