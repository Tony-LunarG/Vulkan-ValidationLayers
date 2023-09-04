/*
 * Copyright (c) 2015-2023 The Khronos Group Inc.
 * Copyright (c) 2015-2023 Valve Corporation
 * Copyright (c) 2015-2023 LunarG, Inc.
 * Copyright (c) 2015-2023 Google, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 */

#pragma once

#include "layer_validation_tests.h"

// Helper class for tersely creating create pipeline tests
//
// Designed with minimal error checking to ensure easy error state creation
// See OneshotTest for typical usage
struct CreatePipelineHelper {
  public:
    std::vector<VkDescriptorSetLayoutBinding> dsl_bindings_;
    std::unique_ptr<OneOffDescriptorSet> descriptor_set_;
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages_;
    VkPipelineVertexInputStateCreateInfo vi_ci_ = {};
    VkPipelineInputAssemblyStateCreateInfo ia_ci_ = {};
    VkPipelineTessellationStateCreateInfo tess_ci_ = {};
    VkViewport viewport_ = {};
    VkRect2D scissor_ = {};
    VkPipelineViewportStateCreateInfo vp_state_ci_ = {};
    VkPipelineMultisampleStateCreateInfo pipe_ms_state_ci_ = {};
    VkPipelineLayoutCreateInfo pipeline_layout_ci_ = {};
    VkPipelineLayoutObj pipeline_layout_;
    VkPipelineDynamicStateCreateInfo dyn_state_ci_ = {};
    VkPipelineRasterizationStateCreateInfo rs_state_ci_ = {};
    VkPipelineRasterizationLineStateCreateInfoEXT line_state_ci_ = {};
    std::vector<VkPipelineColorBlendAttachmentState> cb_attachments_ = {};
    VkPipelineColorBlendStateCreateInfo cb_ci_ = {};
    VkPipelineDepthStencilStateCreateInfo ds_ci_ = {};
    VkGraphicsPipelineCreateInfo gp_ci_ = {};
    VkPipelineCacheCreateInfo pc_ci_ = {};
    VkPipeline pipeline_ = VK_NULL_HANDLE;
    VkPipelineCache pipeline_cache_ = VK_NULL_HANDLE;
    std::unique_ptr<VkShaderObj> vs_;
    std::unique_ptr<VkShaderObj> fs_;
    VkLayerTest &layer_test_;
    std::optional<VkGraphicsPipelineLibraryCreateInfoEXT> gpl_info;
    CreatePipelineHelper(VkLayerTest &test, uint32_t color_attachments_count = 1u);
    ~CreatePipelineHelper();

    VkPipeline Handle() { return pipeline_; }
    void InitDescriptorSetInfo();
    void InitInputAndVertexInfo();
    void InitMultisampleInfo();
    void InitPipelineLayoutInfo();
    void InitViewportInfo();
    void InitDynamicStateInfo();
    void InitShaderInfo();
    void ResetShaderInfo(const char *vertex_shader_text, const char *fragment_shader_text);
    void InitRasterizationInfo();
    void InitLineRasterizationInfo();
    void InitBlendStateInfo();
    void InitGraphicsPipelineInfo();
    void InitPipelineCacheInfo();

    // Not called by default during init_info
    void InitTesselationState();

    // TDB -- add control for optional and/or additional initialization
    void InitInfo();
    void InitState();
    void InitPipelineCache();
    void LateBindPipelineInfo();
    VkResult CreateGraphicsPipeline(bool do_late_bind = true);

    void InitVertexInputLibInfo(void *p_next = nullptr);

    template <typename StageContainer>
    void InitPreRasterLibInfoFromContainer(const StageContainer &stages, void *p_next = nullptr) {
        InitPreRasterLibInfo(static_cast<uint32_t>(stages.size()), stages.data(), p_next);
    }
    void InitPreRasterLibInfo(uint32_t count, const VkPipelineShaderStageCreateInfo *info, void *p_next = nullptr);

    template <typename StageContainer>
    void InitFragmentLibInfoFromContainer(const StageContainer &stages, void *p_next = nullptr) {
        InitFragmentLibInfo(static_cast<uint32_t>(stages.size()), stages.data(), p_next);
    }
    void InitFragmentLibInfo(uint32_t count, const VkPipelineShaderStageCreateInfo *info, void *p_next = nullptr);

    void InitFragmentOutputLibInfo(void *p_next = nullptr);

    // Helper function to create a simple test case (positive or negative)
    //
    // info_override can be any callable that takes a CreatePipelineHeper &
    // flags, error can be any args accepted by "SetDesiredFailure".
    template <typename Test, typename OverrideFunc, typename ErrorContainer>
    static void OneshotTest(Test &test, const OverrideFunc &info_override, const VkFlags flags, const ErrorContainer &errors) {
        CreatePipelineHelper helper(test);
        helper.InitInfo();
        info_override(helper);
        helper.InitState();

        for (const auto &error : errors) test.Monitor().SetDesiredFailureMsg(flags, error);
        helper.CreateGraphicsPipeline();

        if (!errors.empty()) {
            test.Monitor().VerifyFound();
        }
    }

    template <typename Test, typename OverrideFunc>
    static void OneshotTest(Test &test, const OverrideFunc &info_override, const VkFlags flags, const char *error) {
        std::array errors = {error};
        OneshotTest(test, info_override, flags, errors);
    }

    template <typename Test, typename OverrideFunc>
    static void OneshotTest(Test &test, const OverrideFunc &info_override, const VkFlags flags, const std::string &error) {
        std::array errors = {error};
        OneshotTest(test, info_override, flags, errors);
    }

    template <typename Test, typename OverrideFunc>
    static void OneshotTest(Test &test, const OverrideFunc &info_override, const VkFlags flags) {
        std::array<const char *, 0> errors;
        OneshotTest(test, info_override, flags, errors);
    }
};

struct CreateComputePipelineHelper {
  public:
    std::vector<VkDescriptorSetLayoutBinding> dsl_bindings_;
    std::unique_ptr<OneOffDescriptorSet> descriptor_set_;
    VkPipelineLayoutCreateInfo pipeline_layout_ci_ = {};
    VkPipelineLayoutObj pipeline_layout_;
    VkComputePipelineCreateInfo cp_ci_ = {};
    VkPipelineCacheCreateInfo pc_ci_ = {};
    VkPipeline pipeline_ = VK_NULL_HANDLE;
    VkPipelineCache pipeline_cache_ = VK_NULL_HANDLE;
    std::unique_ptr<VkShaderObj> cs_;
    bool override_skip_ = false;
    VkLayerTest &layer_test_;
    CreateComputePipelineHelper(VkLayerTest &test);
    ~CreateComputePipelineHelper();

    void InitDescriptorSetInfo();
    void InitPipelineLayoutInfo();
    void InitShaderInfo();
    void InitComputePipelineInfo();
    void InitPipelineCacheInfo();

    // TDB -- add control for optional and/or additional initialization
    void InitInfo();
    void InitState();
    void InitPipelineCache();
    void LateBindPipelineInfo();
    VkResult CreateComputePipeline(bool do_late_bind = true);

    // Helper function to create a simple test case (positive or negative)
    //
    // info_override can be any callable that takes a CreatePipelineHeper &
    // flags, error can be any args accepted by "SetDesiredFailure".
    template <typename Test, typename OverrideFunc, typename Error>
    static void OneshotTest(Test &test, const OverrideFunc &info_override, const VkFlags flags, const std::vector<Error> &errors,
                            bool positive_test = false) {
        CreateComputePipelineHelper helper(test);
        helper.InitInfo();
        info_override(helper);
        // Allow lambda to decide if to skip trying to compile pipeline to prevent crashing
        if (helper.override_skip_) {
            helper.override_skip_ = false;  // reset
            return;
        }
        helper.InitState();

        for (const auto &error : errors) test.Monitor().SetDesiredFailureMsg(flags, error);
        helper.CreateComputePipeline();

        if (!errors.empty()) {
            test.Monitor().VerifyFound();
        }
    }

    template <typename Test, typename OverrideFunc, typename Error>
    static void OneshotTest(Test &test, const OverrideFunc &info_override, const VkFlags flags, Error error) {
        OneshotTest(test, info_override, flags, std::vector<Error>(1, error));
    }

    template <typename Test, typename OverrideFunc>
    static void OneshotTest(Test &test, const OverrideFunc &info_override, const VkFlags flags) {
        OneshotTest(test, info_override, flags, std::vector<std::string>{});
    }
};

// Helper class for to create ray tracing pipeline tests
// Designed with minimal error checking to ensure easy error state creation
// See OneshotTest for typical usage
struct RayTracingPipelineHelper {
  public:
    std::vector<VkDescriptorSetLayoutBinding> dsl_bindings_;
    std::unique_ptr<OneOffDescriptorSet> descriptor_set_;
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages_;
    VkPipelineLayoutCreateInfo pipeline_layout_ci_ = {};
    vk_testing::PipelineLayout pipeline_layout_;
    VkRayTracingPipelineCreateInfoNV rp_ci_ = {};
    VkRayTracingPipelineCreateInfoKHR rp_ci_KHR_ = {};
    VkPipelineCacheCreateInfo pc_ci_ = {};
    VkPipeline pipeline_ = VK_NULL_HANDLE;
    VkPipelineCache pipeline_cache_ = VK_NULL_HANDLE;
    std::vector<VkRayTracingShaderGroupCreateInfoNV> groups_;
    std::vector<VkRayTracingShaderGroupCreateInfoKHR> groups_KHR_;
    std::unique_ptr<VkShaderObj> rgs_;
    std::unique_ptr<VkShaderObj> chs_;
    std::unique_ptr<VkShaderObj> mis_;
    VkLayerTest& layer_test_;
    RayTracingPipelineHelper(VkLayerTest& test);
    ~RayTracingPipelineHelper();

    void InitShaderGroups();
    void InitShaderGroupsKHR();
    void InitDescriptorSetInfo();
    void InitDescriptorSetInfoKHR();
    void InitPipelineLayoutInfo();
    void InitShaderInfo();
    void InitShaderInfoKHR();
    void InitNVRayTracingPipelineInfo();
    void InitKHRRayTracingPipelineInfo();
    void InitPipelineCacheInfo();
    void InitInfo(bool isKHR = false);
    void InitState();
    void InitPipelineCache();
    void LateBindPipelineInfo(bool isKHR = false);
    VkResult CreateNVRayTracingPipeline(bool do_late_bind = true);
    VkResult CreateKHRRayTracingPipeline(bool do_late_bind = true);
    // Helper function to create a simple test case (positive or negative)
    //
    // info_override can be any callable that takes a CreateNVRayTracingPipelineHelper &
    // flags, error can be any args accepted by "SetDesiredFailure".
    template <typename Test, typename OverrideFunc, typename Error>
    static void OneshotTest(Test& test, const OverrideFunc& info_override, const std::vector<Error>& errors,
                            const VkFlags flags = kErrorBit) {
        RayTracingPipelineHelper helper(test);
        helper.InitInfo();
        info_override(helper);
        helper.InitState();

        for (const auto& error : errors) test.Monitor().SetDesiredFailureMsg(flags, error);
        helper.CreateNVRayTracingPipeline();
        test.Monitor().VerifyFound();
    }

    template <typename Test, typename OverrideFunc, typename Error>
    static void OneshotTest(Test& test, const OverrideFunc& info_override, Error error, const VkFlags flags = kErrorBit) {
        OneshotTest(test, info_override, std::vector<Error>(1, error), flags);
    }

    template <typename Test, typename OverrideFunc>
    static void OneshotPositiveTest(Test& test, const OverrideFunc& info_override, const VkFlags message_flag_mask = kErrorBit) {
        RayTracingPipelineHelper helper(test);
        helper.InitInfo();
        info_override(helper);
        helper.InitState();

        ASSERT_VK_SUCCESS(helper.CreateNVRayTracingPipeline());
    }
};