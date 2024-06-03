#pragma once

#include "cgp/cgp.hpp"
#include "environment_dune.hpp"
#include "objects.hpp"
#include "terrain.hpp"

using cgp::mesh;
using cgp::mesh_drawable;
using cgp::numarray;
using cgp::timer_basic;
using cgp::vec3;

// Variables associated to the GUI (buttons, etc)
struct gui_parameters {
    bool display_frame = true;
    bool display_wireframe = false;
};

// The structure of the custom scene
struct scene_structure : cgp::scene_inputs_generic {
    /************************************/
    /* Elements and shapes of the scene */
    /************************************/

    camera_controller_first_person camera_control;
    camera_projection_perspective camera_projection;
    window_structure window;

    mesh_drawable global_frame;        // The standard global frame
    environment_structure environment; // Standard environment controler
    input_devices inputs; // Storage for inputs status (mouse, keyboard, window
                          // dimension)
    gui_parameters gui;   // Standard GUI element storage

    /************************************/
    /* Elements and shapes of the scene */
    /************************************/

    timer_basic timer;
    Ground ground;
    Cliff cliff;
    Marteleur marteleur;
    WormDust worm_dust;
    Worm worm;

    /*************/
    /* Functions */
    /*************/

    // Standard initialization to be called before the animation loop
    void initialize();

    // The frame display to be called within the animation loop
    void display_frame();

    // The display of the GUI, also called within the animation loop
    void display_gui();

    void mouse_move_event();
    void mouse_click_event();
    void keyboard_event();
    void idle_frame();

    void display_info();
};
