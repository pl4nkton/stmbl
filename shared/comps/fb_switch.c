#include "commands.h"
#include "hal.h"
#include "math.h"
#include "defines.h"
#include "angle.h"

HAL_COMP(fb_switch);

HAL_PIN(polecount);

HAL_PIN(pos_fb);
HAL_PIN(vel_fb);
HAL_PIN(com_fb);
HAL_PIN(joint_fb);
HAL_PIN(state);  // 0 = disabled, 1 = enabled

HAL_PIN(cmd_pos);

HAL_PIN(mot_pos);
HAL_PIN(mot_abs_pos);
HAL_PIN(mot_polecount);
HAL_PIN(mot_offset);
HAL_PIN(mot_state);  // 0 = disabled, 1 = inc, 2 = start abs, 3 = abs
HAL_PIN(mot_rev);
HAL_PIN(mot_fb_no_offset);
HAL_PIN(mot_abs_fb_no_offset);

HAL_PIN(com_pos);
HAL_PIN(com_abs_pos);
HAL_PIN(com_polecount);
HAL_PIN(com_offset);
HAL_PIN(com_state);
HAL_PIN(com_rev);

HAL_PIN(joint_pos);
HAL_PIN(joint_abs_pos);
HAL_PIN(joint_offset);
HAL_PIN(joint_state);
HAL_PIN(joint_rev);
HAL_PIN(joint_fb_no_offset);

HAL_PIN(mot_joint_ratio);

HAL_PIN(phase_time);
HAL_PIN(phase_cur);
HAL_PIN(id);

HAL_PIN(current_com_pos);

HAL_PIN(en);

struct fb_switch_ctx_t {
  int32_t current_com_pos;
  float cmd_offset;
  float com_offset;
  float phase_timer;
  int32_t phase_state;
};

static void nrt_init(volatile void *ctx_ptr, volatile hal_pin_inst_t *pin_ptr) {
  struct fb_switch_ctx_t *ctx      = (struct fb_switch_ctx_t *)ctx_ptr;
  struct fb_switch_pin_ctx_t *pins = (struct fb_switch_pin_ctx_t *)pin_ptr;

  ctx->current_com_pos = 10;
  ctx->cmd_offset      = 0.0;
  ctx->com_offset      = 0.0;
  PIN(phase_cur)       = 1.0;
  PIN(phase_time)      = 1.0;
}

static void rt_func(float period, volatile void *ctx_ptr, volatile hal_pin_inst_t *pin_ptr) {
  struct fb_switch_ctx_t *ctx      = (struct fb_switch_ctx_t *)ctx_ptr;
  struct fb_switch_pin_ctx_t *pins = (struct fb_switch_pin_ctx_t *)pin_ptr;

  float mot_pos   = PIN(mot_pos);
  float com_pos   = PIN(com_pos);
  float joint_pos = PIN(joint_pos);

  float mot_abs_pos   = PIN(mot_abs_pos);
  float com_abs_pos   = PIN(com_abs_pos);
  float joint_abs_pos = PIN(joint_abs_pos);

  float mot_offset   = PIN(mot_offset);
  float com_offset   = PIN(com_offset);
  float joint_offset = PIN(joint_offset);


  if(PIN(mot_rev) > 0.0) {
    mot_pos *= -1.0;
    mot_abs_pos *= -1.0;
    mot_offset *= -1.0;
  }

  if(PIN(com_rev) > 0.0) {
    com_pos *= -1.0;
    com_abs_pos *= -1.0;
    com_offset *= -1.0;
  }

  if(PIN(joint_rev) > 0.0) {
    joint_pos *= -1.0;
    joint_abs_pos *= -1.0;
    joint_offset *= -1.0;
  }

  PIN(mot_fb_no_offset)   = mot_pos;
  PIN(mot_abs_fb_no_offset) = mot_abs_pos;
  PIN(joint_fb_no_offset) = joint_pos;

  PIN(pos_fb) = mod(mot_pos + ctx->cmd_offset);
  PIN(vel_fb) = mot_pos;

  if(PIN(en) <= 0.0) {
    PIN(state)           = 0.0;
    ctx->current_com_pos = 10;
    ctx->com_offset      = 0.0;
    ctx->cmd_offset      = minus(PIN(cmd_pos), mot_pos);
    PIN(pos_fb)          = mod(mot_pos);
    PIN(id)              = 0.0;
  } else {
    PIN(state) = 1.0;
    if(PIN(joint_state) >= 2.0 && ctx->current_com_pos > 3.0) {
      ctx->current_com_pos = 3;
      ctx->com_offset      = minus(mod((joint_abs_pos + joint_offset) * PIN(polecount) / PIN(mot_joint_ratio)), mod(mot_pos * PIN(polecount) / PIN(mot_polecount)));
    }
    if(PIN(com_state) >= 2.0 && ctx->current_com_pos > 2.0) {
      ctx->current_com_pos = 2;
      ctx->com_offset      = minus(mod((com_abs_pos + com_offset) * PIN(polecount) / PIN(com_polecount)), mod(mot_pos * PIN(polecount) / PIN(mot_polecount)));
    }
    if(PIN(mot_state) >= 2.0 && ctx->current_com_pos > 1.0) {
      ctx->current_com_pos = 1;
      ctx->com_offset      = 0.0;
    }
    if(ctx->current_com_pos > 4.0) {
      PIN(com_fb) = 0.0;

      ctx->phase_timer += period;
      PIN(id) = CLAMP(ctx->phase_timer / (MAX(PIN(phase_time), 0.1) / 3.0) * PIN(phase_cur), 0.0, PIN(phase_cur));

      if(ctx->phase_timer >= MAX(PIN(phase_time), 0.1)) {
        ctx->phase_timer     = 0.0;
        ctx->com_offset      = -mod(mot_pos * PIN(polecount) / PIN(mot_polecount));
        ctx->cmd_offset      = minus(PIN(cmd_pos), mot_pos);
        PIN(id)              = 0.0;
        ctx->current_com_pos = 4.0;
      }
    }

    PIN(current_com_pos) = ctx->current_com_pos;

    switch(ctx->current_com_pos) {
      case 4:
        PIN(com_fb) = mod(mot_pos * PIN(polecount) / PIN(mot_polecount) + ctx->com_offset);
        break;

      case 3:
        if(PIN(joint_state) != 3.0) {
          PIN(com_fb) = mod(mot_pos * PIN(polecount) / PIN(mot_polecount) + ctx->com_offset);
        } else {
          PIN(com_fb) = mod((joint_abs_pos + joint_offset) * PIN(polecount));
        }
        break;

      case 2:
        if(PIN(com_state) != 3.0) {
          PIN(com_fb) = mod(mot_pos * PIN(polecount) / PIN(mot_polecount) + ctx->com_offset);
        } else {
          PIN(com_fb) = mod((com_abs_pos + com_offset) * PIN(polecount) / PIN(com_polecount));
        }
        break;

      case 1:
        if(PIN(mot_state) != 3.0) {
          PIN(state) = 0.0;
        } else {
          PIN(com_fb) = mod((mot_abs_pos + mot_offset) * PIN(polecount) / PIN(mot_polecount));
        }
        break;

      default:
        PIN(state) = 0.0;
    }
  }
}

hal_comp_t fb_switch_comp_struct = {
    .name      = "fb_switch",
    .nrt       = 0,
    .rt        = rt_func,
    .frt       = 0,
    .nrt_init  = nrt_init,
    .rt_start  = 0,
    .frt_start = 0,
    .rt_stop   = 0,
    .frt_stop  = 0,
    .ctx_size  = sizeof(struct fb_switch_ctx_t),
    .pin_count = sizeof(struct fb_switch_pin_ctx_t) / sizeof(struct hal_pin_inst_t),
};
