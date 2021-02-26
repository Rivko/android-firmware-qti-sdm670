#!/bin/bash

set -o errexit

usage() {
cat <<- EOF

    Sync Proprietary from Grease and OSS content from CAF.

    Usage:
        For Proprietary + OSS: sync.sh --shallow-clone --quiet --jobs 8 -u <username> \
                               -p <password> -b <grease_branch> -g <grease_server>
        For OSS: sync.sh --shallow-clone --quiet --jobs 8

    Options:
        a) Fetch all branches instead of just pulling the branch specified in
           the manifest. It is NOT recommended to specify this flag as it can
           substantially increase the sync time.
        q) Reduce verbosity
        s) Create a shallow clone with depth 1
        t) Fetch tags
        j) Number of projects to fetch simultaneously (defaults to 4)
        u) Username for Grease Access.
        p) Password for Grease Access.
        b) Grease branch
        g) Grease server

EOF

exit 1
}

# Defaults

quiet=false
current_branch=true
shallow_clone=false
no_tags=true
mainline=false
repo_jobs=4
mainline_au='::mainline_au::'
au='::au::'
grease_utilities_revision='::grease_utilities_revision::'
caf_server='::caf_server::'
caf_manifest_repo='::caf_manifest_repo::'
manifest_url='git://codeaurora.org/quic/la/platform/manifest'
manifest_file='caf_AU_LINUX_ANDROID_LA.UM.6.8.1.R3.08.01.00.575.035.xml'
manifest_branch='release'
repo_url='git://codeaurora.org/tools/repo.git'
repo_branch='caf-stable'

long_opts="fetch-all-branches,shallow-clone,quiet,mainline,fetch-tags,jobs:,username:,password:branch:grease_server:"

if ! getopts=$(getopt -o asqmtj:u:p:b:g: -l $long_opts -- "$@"); then
   echo "Error processing options"
   usage
fi

eval set -- "$getopts"

while true; do
    case "$1" in
       -a|--fetch-all-branches) current_branch=false;;
       -s|--shallow-clone) shallow_clone=true;;
       -q|--quiet) quiet=true;;
       -m|--mainline) mainline=true;;
       -t|--fetch-tags) no_tags=false;;
       -j|--jobs) repo_jobs=$2 ; shift;;
       -u|--username) username=$2; shift;;
       -p|--password) password=$2; shift;;
       -b|--branch) branch=$2; shift;;
       -g|--grease_server) grease_server=$2; shift;;
       --) shift ; break ;;
       *) echo "Error processing args -- unrecognized option $1" >&2
          usage;;
    esac
    shift
done

sync_args=()
init_args=()
build_tree="$(dirname $(readlink -f $0))"

if ! [[ $repo_jobs =~ ^[0-9]+$ ]] ; then
    echo 'ERROR: value passed to --jobs must be an integer'
    usage
fi

if $quiet; then
    sync_args+=('--quiet')
fi

if $current_branch; then
    sync_args+=('--current-branch')
fi

if $no_tags; then
    sync_args+=('--no-tags')
fi

if $shallow_clone; then
    if ! repo init --help | grep -q '\-\-depth'; then
        echo "ERROR: Current version of repo doesn't support shallow clone."
        echo "Please upgrade repo to use this option"
        usage
    fi
    init_args+=('--depth 1')
fi

if [[ ! -z $username ]] && [[ ! -z $password ]];then
    build_tree="$(dirname $(readlink -f $0))/.."
    grease_utilities_branch="remotes/origin/$(echo $branch | sed 's/\//\/project\//g')/$grease_utilities_revision"
    caf_server=$(echo $caf_server | sed 's/ssh:\/\/git\.codeaurora\.org/ssh:\/\/git\@git\.codeaurora\.org/g')
    ls --ignore sync.sh | xargs rm -rf
    cd "$build_tree"
    ls --ignore android | xargs rm -rf
    git clone ssh://$username@grease-sd-stg.qualcomm.com:29418/platform/vendor/qcom-proprietary/grease/utilities grease_utilities
    cd grease_utilities
    git checkout $grease_utilities_branch
    cd ..
    cp -a grease_utilities/sync_all.sh grease_utilities/join_manifests.sh ./
    if $mainline;then
        ./sync_all.sh -a $mainline_au -b $branch -u $username -p $password \
                  -s $grease_server -c $caf_manifest_repo -l $caf_server
    else
        ./sync_all.sh -a $au -b $branch -u $username -p $password \
                  -s $grease_server -c $caf_manifest_repo -l $caf_server
    fi
    ls --ignore android | xargs rm -rf
    echo "Sync Completed Successfully for Proprietary and OSS projects"
else
    cd "$build_tree"
    manifest_url=$(echo $manifest_url | sed 's/ssh:\/\/git\.codeaurora\.org/ssh:\/\/git\@git\.codeaurora\.org/g')
    repo init --manifest-url $manifest_url \
              --manifest-name $manifest_file \
              --manifest-branch $manifest_branch \
              --repo-url=$repo_url \
              --repo-branch=$repo_branch \
               ${init_args[@]}
    repo sync --network-only --jobs=$repo_jobs ${sync_args[@]}
    repo sync --detach --local-only --jobs=$repo_jobs ${sync_args[@]}

    echo 'Sync Completed Successfully'
fi
