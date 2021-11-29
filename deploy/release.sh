#!/bin/sh

set -e

TAG=
REF=
URL=
JOB=
FILE=

abort()
{
    echo "$@" >&2
    exit 1
}

get_asset_link()
{
    file="$1"
    name="$(basename "$file")"
    kind="package"
    path="/packages/$name"
    url="$URL/$file?job=$JOB"
    echo "{\"name\":\"$name\",\"url\":\"$url\",\"filepath\":\"$path\",\"link_type\":\"$kind\"}"
}

show_help()
{
    cat <<EOF
Usage:
  $(basename "$0") [-h] --tag TAG --ref SHA --url URL --job JOB --version-file FILE

Options:
  -h|--help                 show this message
  -t|--tag TAG              release tag name
  -r|--ref REF              release commit reference
  -u|--url URL              pipeline artifacts base url
  -j|--job JOB              pipeline job to fetch artifacts from
  -f|--version-file FILE    ini file containing version
EOF
}

ARGS=

while [ -n "$1" ]
do
    case $1 in
        --) shift; break;;
        -*) case $1 in
            -h|--help)          show_help; exit 0;;
            -t|--tag)           TAG=$2; shift;;
            -r|--ref)           REF=$2; shift;;
            -u|--url)           URL=$2; shift;;
            -j|--job)           JOB=$2; shift;;
            -f|--version-file)  FILE=$2; shift;;
            --*)                show_help; exit 1;;
            esac;;
        *) ARGS="$ARGS $1";;
    esac
    shift
done

eval set -- "$ARGS" "$@"

# Check required arguments.
[ -n "$TAG" ] || abort "Empty argument: tag"
[ -n "$REF" ] || abort "Empty argument: ref"
[ -n "$URL" ] || abort "Empty argument: url"
[ -n "$JOB" ] || abort "Empty argument: job"

# Check file with version exists.
[ -r "$FILE" ] || abort "File not found: $FILE"

major=
minor=
patch=

# Check version against tag.
echo "Extracting version from $FILE..."
eval "$(grep "=" version_arke.ini)"

VERSION=$major.$minor.$patch
[ -n "$VERSION" ] || abort "Unable to find version"
if [[ "$TAG" == *"$VERSION"* ]]; then
  echo "Version is correct: $VERSION"
else
  abort "Tag $TAG does not contain $VERSION"
fi

# Extract release notes from git log
echo "Extracting release notes [$TAG]..."
TARGET=${TAG%/v/$major.$minor.$patch*}
git log --pretty=format:"* %s" ${TARGET}/v/$major.$minor.$(shell echo $patch-1 | bc)..HEAD > release-notes.md

echo "Release notes:"
cat release-notes.md

# Generate assets link json for each file argument.
ASSETS=""
for file in "$@"
do
    echo "Generating asset link for $file..."
    ASSETS="$ASSETS --assets-link $(get_asset_link "$file")"
done;

# Create a release for given tag.
# shellcheck disable=SC2086
release-cli create \
    --name "Release $TAG" \
    --tag-name "$TAG" --ref "$REF" \
    --description release-notes.md \
    $ASSETS  # It must not be double-quoted!
