links_dir="$(dirname $(readlink -e "${BASH_SOURCE[0]}"))"
links_path="$(dirname $(readlink -e "${BASH_SOURCE[0]}"))/links"
sys_links_path="$(dirname $(readlink -e "${BASH_SOURCE[0]}"))/system_links"
ven_links_path="$(dirname $(readlink -e "${BASH_SOURCE[0]}"))/vendor_links"
failed_links=""
count=0
if [[ -f "$links_path" || -f "$sys_links_path" || -f "$ven_links_path" ]];then
  total_links=$(cat $links_dir/*links| wc -l)
  for i in $(cat $links_dir/*links);do
    src=$(echo $i | awk -F:: '{print $1}')
    dest=$(echo $i | awk -F:: '{print $2}')
    if [ -e "$dest" ];then
      rm -rf $dest
    fi
    if [[ -e $src && ! -e $dest ]];then
      mkdir -p $(dirname $dest)
      ln -srf $src $dest
      count=$(($count + 1))
    else
      failed_links="$failed_links $i"
    fi
  done
  if [ ! -z "$failed_links" ];then
    echo "*****Could not create symlink*******"
    echo $failed_links | sed 's/[[:space:]]/\n/g'
    echo "****************END******************"
  fi
  echo "Created $count symlinks out of $total_links mapped links.."
fi
