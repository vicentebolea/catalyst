# Updating CI images

CI images for Linux are stored within this directory. Each subdirectory is a
different image.

## Layout

Each image should use a shell script to perform its actions rather than bare
`RUN` commands in the `Dockerfile`. The rationale is that:

  - commands can be commented
  - caches can easily be cleaned up between image layers
  - the script exists in the image for documentation of how it was built

## Building

No arguments should be required to build these images. Note that if `podman` is
used, the `--format docker` flag is needed to create compatible images.

## Tagging scheme

CI images should be tagged as:

    kitware/paraview:ci-catalyst-DIRECTORY-PLATFORM-YYYYMMDD

where `DIRECTORY` is the name of the directory and `YYYYMMDD` is the date the
image is built. PLATFORM refers to the image platform using the naming that
docker uses when referring to different platforms. If multiple images are
committed to `master` a day, use an additional `.N` suffix where `N` is a
sequential integer.

## Pushing images

Pushing images requires a token in order to push to the Kitware repository.
Please tag the `@utils/maintainers/dockerhub` to request pushing your image to
Kitware's dockerhub.

## Updating CI

The image tag needs updated in `.gitlab/os-linux.yml`.

## Notes about building different architecture images

Only the Fedora image is currently intended to be built using an arch other
than x86_64. If your host system has a different arch than the target container
image you might need to use docker buildx to build your image, find the
instructions in how to do it at:
https://docs.docker.com/build/building/multi-platform/.
