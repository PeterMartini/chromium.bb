# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'cc_source_files': [
      'hash_pair.h',
      'scoped_ptr_deque.h',
      'scoped_ptr_hash_map.h',
      'scoped_ptr_vector.h',
      'bitmap_content_layer_updater.cc',
      'bitmap_content_layer_updater.h',
      'bitmap_skpicture_content_layer_updater.cc',
      'bitmap_skpicture_content_layer_updater.h',
      'caching_bitmap_content_layer_updater.cc',
      'caching_bitmap_content_layer_updater.h',
      'active_animation.cc',
      'active_animation.h',
      'append_quads_data.h',
      'animation_curve.cc',
      'animation_curve.h',
      'animation_events.h',
      'checkerboard_draw_quad.cc',
      'checkerboard_draw_quad.h',
      'completion_event.h',
      'damage_tracker.cc',
      'damage_tracker.h',
      'debug_border_draw_quad.cc',
      'debug_border_draw_quad.h',
      'debug_rect_history.cc',
      'debug_rect_history.h',
      'delay_based_time_source.cc',
      'delay_based_time_source.h',
      'delegated_renderer_layer_impl.cc',
      'delegated_renderer_layer_impl.h',
      'direct_renderer.cc',
      'direct_renderer.h',
      'draw_quad.cc',
      'draw_quad.h',
      'font_atlas.cc',
      'font_atlas.h',
      'frame_rate_controller.cc',
      'frame_rate_controller.h',
      'frame_rate_counter.cc',
      'frame_rate_counter.h',
      'graphics_context.h',
      'heads_up_display_layer_impl.cc',
      'heads_up_display_layer_impl.h',
      'io_surface_draw_quad.cc',
      'io_surface_draw_quad.h',
      'io_surface_layer_impl.cc',
      'io_surface_layer_impl.h',
      'input_handler.h',
      'keyframed_animation_curve.cc',
      'keyframed_animation_curve.h',
      'layer_animation_controller.cc',
      'layer_animation_controller.h',
      'layer_impl.cc',
      'layer_impl.h',
      'layer_iterator.cc',
      'layer_iterator.h',
      'layer_quad.cc',
      'layer_quad.h',
      'layer_sorter.cc',
      'layer_sorter.h',
      'layer_tiling_data.cc',
      'layer_tiling_data.h',
      'layer_tree_host.cc',
      'layer_tree_host.h',
      'layer_tree_host_client.h',
      'layer_tree_host_common.cc',
      'layer_tree_host_common.h',
      'layer_tree_host_impl.cc',
      'layer_tree_host_impl.h',
      'managed_memory_policy.cc',
      'managed_memory_policy.h',
      'math_util.cc',
      'math_util.h',
      'occlusion_tracker.cc',
      'occlusion_tracker.h',
      'overdraw_metrics.cc',
      'overdraw_metrics.h',
      'page_scale_animation.cc',
      'page_scale_animation.h',
      'prioritized_texture.cc',
      'prioritized_texture.h',
      'prioritized_texture_manager.cc',
      'prioritized_texture_manager.h',
      'priority_calculator.cc',
      'priority_calculator.h',
      'proxy.cc',
      'proxy.h',
      'quad_culler.cc',
      'quad_culler.h',
      'quad_sink.h',
      'render_pass.cc',
      'render_pass.h',
      'render_pass_draw_quad.cc',
      'render_pass_draw_quad.h',
      'render_pass_sink.h',
      'render_surface_impl.cc',
      'render_surface_impl.h',
      'render_surface_filters.cc',
      'render_surface_filters.h',
      'renderer.cc',
      'renderer.h',
      'gl_renderer.cc',
      'gl_renderer.h',
      'software_renderer.cc',
      'software_renderer.h',
      'rendering_stats.h',
      'resource_provider.cc',
      'resource_provider.h',
      'resource_update.cc',
      'scheduler.cc',
      'scheduler.h',
      'scheduler_state_machine.cc',
      'scheduler_state_machine.h',
      'scoped_texture.cc',
      'scoped_texture.h',
      'scoped_thread_proxy.cc',
      'scoped_thread_proxy.h',
      'scrollbar_animation_controller.cc',
      'scrollbar_animation_controller.h',
      'scrollbar_animation_controller_linear_fade.cc',
      'scrollbar_animation_controller_linear_fade.h',
      'scrollbar_layer_impl.cc',
      'scrollbar_layer_impl.h',
      'scrollbar_geometry_fixed_thumb.cc',
      'scrollbar_geometry_fixed_thumb.h',
      'scrollbar_geometry_stub.cc',
      'scrollbar_geometry_stub.h',
      'settings.cc',
      'settings.h',
      'shared_quad_state.cc',
      'shared_quad_state.h',
      'single_thread_proxy.cc',
      'single_thread_proxy.h',
      'solid_color_draw_quad.cc',
      'solid_color_draw_quad.h',
      'solid_color_layer_impl.cc',
      'solid_color_layer_impl.h',
      'stream_video_draw_quad.cc',
      'stream_video_draw_quad.h',
      'texture.cc',
      'texture.h',
      'texture_draw_quad.cc',
      'texture_draw_quad.h',
      'texture_layer_impl.cc',
      'texture_layer_impl.h',
      'resource_update_controller.cc',
      'resource_update_controller.h',
      'resource_update_queue.cc',
      'resource_update_queue.h',
      'thread.h',
      'thread_proxy.cc',
      'thread_proxy.h',
      'thread_task.h',
      'tile_draw_quad.cc',
      'tile_draw_quad.h',
      'tiled_layer_impl.cc',
      'tiled_layer_impl.h',
      'time_source.h',
      'timer.cc',
      'timer.h',
      'timing_function.cc',
      'timing_function.h',
      'video_layer_impl.cc',
      'video_layer_impl.h',
      'yuv_video_draw_quad.cc',
      'yuv_video_draw_quad.h',
      'content_layer_updater.cc',
      'content_layer_updater.h',
      'content_layer.cc',
      'content_layer.h',
      'content_layer_client.h',
      'delegated_renderer_layer.cc',
      'delegated_renderer_layer.h',
      'geometry_binding.cc',
      'geometry_binding.h',
      'heads_up_display_layer.cc',
      'heads_up_display_layer.h',
      'io_surface_layer.cc',
      'io_surface_layer.h',
      'image_layer.cc',
      'image_layer.h',
      'layer.cc',
      'layer.h',
      'layer_painter.h',
      'layer_updater.cc',
      'layer_updater.h',
      'platform_color.h',
      'program_binding.cc',
      'program_binding.h',
      'rate_limiter.cc',
      'rate_limiter.h',
      'render_surface.cc',
      'render_surface.h',
      'scrollbar_layer.cc',
      'scrollbar_layer.h',
      'shader.cc',
      'shader.h',
      'skpicture_content_layer_updater.cc',
      'skpicture_content_layer_updater.h',
      'solid_color_layer.cc',
      'solid_color_layer.h',
      'switches.cc',
      'switches.h',
      'texture_copier.cc',
      'texture_copier.h',
      'texture_layer.cc',
      'texture_layer.h',
      'texture_layer_client.h',
      'texture_uploader.cc',
      'texture_uploader.h',
      'tiled_layer.cc',
      'tiled_layer.h',
      'tree_synchronizer.cc',
      'tree_synchronizer.h',
      'video_layer.cc',
      'video_layer.h',

      'resource_update.h',
      'threaded_test.h',
    ],
  },
  'targets': [
    {
      'target_name': 'cc',
      'type': 'static_library',
      'includes': [
        'cc.gypi',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
        '<(DEPTH)/skia/skia.gyp:skia',
        '<(DEPTH)/ui/gl/gl.gyp:gl',
        '<(DEPTH)/ui/ui.gyp:ui',
        '<(webkit_src_dir)/Source/WTF/WTF.gyp/WTF.gyp:wtf',
        '<(webkit_src_dir)/Source/WebCore/WebCore.gyp/WebCore.gyp:webcore_platform_geometry',
        '<(webkit_src_dir)/Source/WebKit/chromium/WebKit.gyp:webkit_wtf_support',
      ],
      'defines': [
        # http://crbug.com/154052
        'WEBKIT_GLUE_IMPLEMENTATION=1',
      ],
      'include_dirs': [
        '<(webkit_src_dir)/Source/Platform/chromium',
        '<@(cc_stubs_dirs)',
      ],
      'sources': [
        '<@(cc_source_files)',
        'stubs/FloatPoint.h',
        'stubs/FloatPoint3D.h',
        'stubs/FloatQuad.h',
        'stubs/FloatRect.h',
        'stubs/FloatSize.h',
        'stubs/IntPoint.h',
        'stubs/IntRect.h',
        'stubs/IntSize.h',
        'stubs/NotImplemented.h',
        'stubs/Region.h',
        'stubs/SkiaUtils.h',
        'stubs/TilingData.h',
        'stubs/UnitBezier.h',

        'stubs/config.h',
        'stubs/float_point_3d.h',
        'stubs/float_point.h',
        'stubs/float_quad.h',
        'stubs/float_rect.h',
        'stubs/float_size.h',
        'stubs/int_point.h',
        'stubs/int_rect.h',
        'stubs/int_size.h',
        'stubs/not_implemented.h',
        'stubs/skia_utils.h',
        'stubs/tiling_data.h',
        'stubs/unit_bezier.h',
      ],
    },
  ],
}
