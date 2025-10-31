#!/usr/bin/env bash


PLOTS_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$PLOTS_DIR/.." && pwd)"
: '
"$0" → represents the current script file: run_root_plots.sh
dirname "$0" → extracts the path of the directory containing this script location : plots/run_root_plots.sh
cd ... && pwd → converts that into an absolute path (stored in PLOTS_DIR)

Then we move one directory up (..) from plots/ to get to the project root folder.
This is stored in ROOT_DIR.
'



declare -A CITY=([A]="Lund" [B]="Uppsala")
: '
puts A as Lund and B as Uppsala, this helps us but making it convinient to indentify files
'

mkdir -p "$ROOT_DIR/figures"
: '
makes a directory to store our figures in case it does not exist
-p makes sure if the directory exists there is not error
'



pushd "$ROOT_DIR" >/dev/null
: '
pushd moves the current working directory to the project root (ROOT_DIR).
>/dev/null hides the printed output of this command to keep the terminal clean.

This is important because the ROOT plotting macro saves images using relative paths like: figures/monthly_bar_Lund_1961.png
so it must be executed from the project root for the paths to resolve correctly.
'



for year in 1961 2024; do
  for station in A B; do
    city="${CITY[$station]}"
    echo "Plotting $city ($station), $year..."
    root -l -b -q "plots/plot_monthly_using_csv_data.C+(\"results/monthly_${city}_${year}.csv\",\"${city}\",${year})"
  done
done

popd >/dev/null
: '
Returns the working directory to wherever the script was initially run from.
>/dev/null again hides the printed directory stack output.
'
echo "All plots saved in $ROOT_DIR/figures/"


: '
How pushd and popd works 

pushd and popd help navigate directories while remembering your previous location.

→ pushd <dir>
   - Moves you INTO <dir>.
   - Saves your CURRENT directory on a "directory stack" (a memory list).
→ popd
   - Pops the last saved directory off the stack and returns you to it.

Why use them ?
In this script:
   pushd "$ROOT_DIR" >/dev/null
   ...
   popd >/dev/null

   pushd "$ROOT_DIR" 
   - Changes the working directory to the project root folder.
   - This ensures all relative paths (like results/... and figures/...) work correctly when ROOT is executed.

  popd
   - Returns to the original folder where the script was started.
   - This keeps your terminal location unchanged after the script finishes.

   >/dev/null 
   - Suppresses the automatic directory printout that pushd and popd would normally show in the terminal.

Ex:
We are in:
    /home/user/Documents/Final_Prj/MNXB11-Group3-Project

Run:
    pushd plots
We move to:
    /home/user/Documents/Final_Prj/MNXB11-Group3-Project/plots
and our previous location is saved internally.

Then run:
    popd
Now we are automatically returned to:
    /home/user/Documents/Final_Prj/MNXB11-Group3-Project

This means:
   - pushd lets us "temporarily go somewhere"
   - popd brings us "back to where you started"
   - Together they help scripts navigate directories safely without losing track of the original location.

In short : 
we first pushd , 
we get the plots store it the figures 
then popd and go back to our original file without the addtional files creates in making the plot

'
