# build image & run
docker build -t sfm_pipeline:latest .
docker run --rm \
# if gpus are available
# =====================
    --gpus all \
# =====================
    -v $(pwd)/db \
    sfm_pipeline:latest \
    $(pwd)/db/images \
    $(pwd)/db/database.db \
    $(pwd)/db/points.pnts