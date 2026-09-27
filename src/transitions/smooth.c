/**
 * Smooth Transition Effects
 *
 * Lightweight single-pass GLSL transitions:
 *   - zoom
 *   - side_wipe
 *   - circle
 *   - radial
 *
 * All effects share one shader program to keep GPU resource usage low.
 */

#include <GL/gl.h>
#include <math.h>

#include "neowall/neowall.h"
#include "neowall/constants.h"
#include "neowall/transitions.h"
#include "neowall/shader/shader.h"

static const char *smooth_vertex_shader_source =
    GLSL_VERSION_STRING
    "in vec2 position;\n"
    "in vec2 texcoord;\n"
    "out vec2 v_texcoord;\n"
    "void main() {\n"
    "    gl_Position = vec4(position, 0.0, 1.0);\n"
    "    v_texcoord = texcoord;\n"
    "}\n";

static const char *smooth_fragment_shader_source =
    GLSL_VERSION_STRING
    "in vec2 v_texcoord;\n"
    "out vec4 fragColor;\n"
    "uniform sampler2D texture0;\n"
    "uniform sampler2D texture1;\n"
    "uniform float progress;\n"
    "uniform float effect;\n"
    "uniform vec2 resolution;\n"
    "\n"
    "vec2 clamp_uv(vec2 uv) {\n"
    "    return clamp(uv, vec2(0.001), vec2(0.999));\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    vec2 uv = v_texcoord;\n"
    "    vec2 center = vec2(0.5);\n"
    "    float p = progress;\n"
    "\n"
    "    vec4 old_color;\n"
    "    vec4 new_color;\n"
    "    float mask;\n"
    "\n"
    "    /* ---------------------------------------------\n"
    "       Zoom: old image gently pulls away while the\n"
    "       new image settles into place.\n"
    "       --------------------------------------------- */\n"
    "    if (effect < 0.5) {\n"
    "        float old_scale = mix(1.0, 0.76, p);\n"
    "        float new_scale = mix(1.18, 1.0, p);\n"
    "\n"
    "        vec2 old_uv = center + (uv - center) / old_scale;\n"
    "        vec2 new_uv = center + (uv - center) / new_scale;\n"
    "\n"
    "        old_color = texture(texture0, clamp_uv(old_uv));\n"
    "        new_color = texture(texture1, clamp_uv(new_uv));\n"
    "        fragColor = mix(old_color, new_color, p);\n"
    "        return;\n"
    "    }\n"
    "\n"
    "    /* ---------------------------------------------\n"
    "       Side wipe: soft horizontal reveal.\n"
    "       --------------------------------------------- */\n"
    "    if (effect < 1.5) {\n"
    "        float edge = mix(-0.05, 1.05, p);\n"
    "        float feather = 0.055;\n"
    "        mask = smoothstep(edge - feather, edge + feather, uv.x);\n"
    "\n"
    "        old_color = texture(texture0, uv);\n"
    "        new_color = texture(texture1, uv);\n"
    "        fragColor = mix(old_color, new_color, mask);\n"
    "        return;\n"
    "    }\n"
    "\n"
    "    /* ---------------------------------------------\n"
    "       Circle: expanding circular reveal.\n"
    "       --------------------------------------------- */\n"
    "    if (effect < 2.5) {\n"
    "        vec2 aspect = vec2(resolution.x / resolution.y, 1.0);\n"
    "        float dist = length((uv - center) * aspect);\n"
    "        float radius = mix(0.0, 0.78, p);\n"
    "        float feather = 0.045;\n"
    "\n"
    "        mask = smoothstep(radius - feather, radius + feather, dist);\n"
    "\n"
    "        old_color = texture(texture0, uv);\n"
    "        new_color = texture(texture1, uv);\n"
    "        fragColor = mix(new_color, old_color, mask);\n"
    "        return;\n"
    "    }\n"
    "\n"
    "    /* ---------------------------------------------\n"
    "       Radial: clockwise angular reveal.\n"
    "       --------------------------------------------- */\n"
    "    {\n"
    "        vec2 aspect = vec2(resolution.x / resolution.y, 1.0);\n"
    "        vec2 pos = (uv - center) * aspect;\n"
    "        float angle = atan(pos.y, pos.x);\n"
    "        float normalized = (angle + 3.14159265) / 6.28318530;\n"
    "        float sweep = fract(normalized + 0.25);\n"
    "        float feather = 0.035;\n"
    "\n"
    "        mask = smoothstep(p - feather, p + feather, sweep);\n"
    "\n"
    "        old_color = texture(texture0, uv);\n"
    "        new_color = texture(texture1, uv);\n"
    "        fragColor = mix(new_color, old_color, mask);\n"
    "    }\n"
    "}\n";

bool shader_create_smooth_transition_program(GLuint *program) {
    return shader_create_program_from_sources(
        smooth_vertex_shader_source,
        smooth_fragment_shader_source,
        program
    );
}

static bool render_smooth(struct output_state *output,
                          float progress,
                          float effect) {
    if (!output || !output->current_image || !output->next_image) {
        return false;
    }

    if (output->texture == 0 || output->next_texture == 0) {
        return false;
    }

    if (output->smooth_transition_program == 0) {
        return false;
    }

    transition_context_t ctx;

    if (!transition_begin(
            &ctx,
            output,
            output->smooth_transition_program)) {
        return false;
    }

    float resolution[2] = {
        (float)output->width,
        (float)output->height
    };

    GLint effect_loc =
        glGetUniformLocation(
            output->smooth_transition_program,
            "effect");

    if (effect_loc >= 0) {
        glUniform1f(effect_loc, effect);
    }

    bool ok = transition_draw_blended_textures(
        &ctx,
        output->next_texture,
        output->texture,
        progress,
        0.0f,
        resolution);

    transition_end(&ctx);

    return ok;
}

bool transition_zoom_render(struct output_state *output, float progress) {
    return render_smooth(output, progress, 0.0f);
}

bool transition_side_wipe_render(struct output_state *output, float progress) {
    return render_smooth(output, progress, 1.0f);
}

bool transition_circle_render(struct output_state *output, float progress) {
    return render_smooth(output, progress, 2.0f);
}

bool transition_radial_render(struct output_state *output, float progress) {
    return render_smooth(output, progress, 3.0f);
}
