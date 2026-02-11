#pragma once

#include <colmap/controllers/incremental_pipeline.h>
#include <colmap/controllers/feature_extraction.h>
#include <colmap/controllers/feature_matching.h>
#include <colmap/feature/pairing.h>
#include <colmap/estimators/two_view_geometry.h>
#include <colmap/scene/reconstruction_manager.h>
#include <colmap/util/logging.h>
#include <colmap/util/controller_thread.h>

#include <string>
#include <vector>
#include <memory>
#include <Eigen/Core>

inline std::vector<Eigen::Vector3d> runSfM(
    const std::string& database_path,
    const std::string& image_path)
{
    // ---- glog init ----
    char arg0[] = "sfm_app";
    char* argv[] = {arg0};
    colmap::InitializeGlog(argv);

    // ---- Feature extraction ----
    //
    // 3.13 API:
    //   CreateFeatureExtractorController(database_path, reader_opts, extraction_opts)
    //
    // ImageReaderOptions no longer carries database_path;
    // it is passed as a separate first argument.
    // SiftExtractionOptions is now a sub-object inside FeatureExtractionOptions.
    colmap::ImageReaderOptions reader_options;
    reader_options.image_path = image_path;

    colmap::FeatureExtractionOptions extraction_options;
    extraction_options.use_gpu = false;
    // extraction_options.use_gpu is set by COLMAP_GPU_ENABLED at build time

    auto extractor = colmap::CreateFeatureExtractorController(
        database_path, reader_options, extraction_options);
    extractor->Start();
    extractor->Wait();

    // ---- Feature matching ----
    //
    // 3.13 API:
    //   CreateExhaustiveFeatureMatcher(pairing_opts, matching_opts,
    //                                  geometry_opts, database_path)
    //
    // The old two-argument form (reader_opts, sift_matching_opts) is gone.
    // SiftMatchingOptions is now a sub-object inside FeatureMatchingOptions.
    colmap::ExhaustivePairingOptions pairing_options;
    colmap::FeatureMatchingOptions  matching_options;
    matching_options.use_gpu = false;
    colmap::TwoViewGeometryOptions  geometry_options;

    auto matcher = colmap::CreateExhaustiveFeatureMatcher(
        pairing_options, matching_options, geometry_options, database_path);
    matcher->Start();
    matcher->Wait();

    // ---- Incremental mapping ----
    //
    // 3.13 API:
    //   IncrementalPipeline(shared_ptr<options>, image_path,
    //                       database_path, shared_ptr<ReconstructionManager>)
    //
    // IncrementalMapperOptions → IncrementalPipelineOptions.
    // CreateIncrementalMapperController() is gone; construct the pipeline
    // directly and wrap in ControllerThread to run it as a thread.
    auto pipeline_options =
        std::make_shared<colmap::IncrementalPipelineOptions>();
    auto reconstruction_manager =
        std::make_shared<colmap::ReconstructionManager>();

    auto pipeline = std::make_shared<colmap::IncrementalPipeline>(
        pipeline_options, image_path, database_path, reconstruction_manager);

    colmap::ControllerThread<colmap::IncrementalPipeline> mapper_thread(pipeline);
    mapper_thread.Start();
    mapper_thread.Wait();

    // ---- Extract 3D points ----
    if (reconstruction_manager->Size() == 0)
        return {};

    // Get(idx) returns shared_ptr<const Reconstruction> in 3.13
    const auto& reconstruction = reconstruction_manager->Get(0);
    std::vector<Eigen::Vector3d> points;
    for (const auto& [id, point] : reconstruction->Points3D())
        points.push_back(point.xyz);

    return points;
}
