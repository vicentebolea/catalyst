#!/bin/sh

set -e

if [ -z "$CI_COMMIT_TAG" ]; then
    echo >&2 'This script is only meant to run for tag pipelines'
    exit 1
fi

readonly urlbase="${CI_API_V4_URL}/projects/${CI_PROJECT_ID}"

upload_file () {
    file="$1"
    shift

    curl --header "JOB-TOKEN: ${CI_JOB_TOKEN}" \
        --upload-file "$file" \
        "$urlbase/packages/generic/catalyst/${CI_COMMIT_TAG}/$file"
}

for file_to_upload in $UPLOAD_FILES; do
    if ! [ -f "$file_to_upload" ]; then
        echo >&2 "Could not find file: '$file_to_upload'"
        exit 1
    fi
    upload_file "$file_to_upload"
done
