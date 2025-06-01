# Use the ue4-full image as the base
FROM adamrehn/ue4-full:4.27.2-cudagl11.2.2-ubuntu18.04

# Install ue4-ci-helpers
RUN pip install --no-cache-dir ue4-ci-helpers
