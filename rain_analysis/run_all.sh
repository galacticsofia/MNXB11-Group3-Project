#!/bin/bash
set -e
: '
set -e ensures that if any command fails, the script stops immediately.
This prevents half-completed runs and ensures errors are not ignored.
'
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

: '
${BASH_SOURCE[0]} → path of the current script file (run_all.sh)
dirname "${BASH_SOURCE[0]}" → extracts the directory path where the script resides
cd ... && pwd → converts this into an absolute path

'

#Data Cleaning

echo "==> Cleaning data..."
cd "$SCRIPT_DIR/data_clean/"

: '
We move into the data_clean/ directory where Rain_data_clean.cxx is located.
- Compiles the .cxx cleaning script
- Runs it to produce a cleaned CSV file
The cleaned file will later be used for analysis.
'

g++ Rain_data_clean.cxx -o Rain_data_clean
: '
Compiles the C++ file "Rain_data_clean.cxx" into an executable called "Rain_data_clean".
g++ : GNU C++ compiler
-o  : specifies the output executable name
'

./Rain_data_clean
: '
Executes the compiled binary to perform the cleaning process.
This will read the raw dataset from datasets/ and produce a cleaned CSV file
(typically stored under data_clean/ or results/).
'


#Data Analysis

echo "==> Running analysis..."
cd "$SCRIPT_DIR/analysis/"

: '
Moves to the analysis/ directory where the analysis code and script are located.
This step compiles analysis.cxx and runs a helper bash script that executes analyses
for multiple years and stations (A → Lund, B → Uppsala).
'

g++ analysis.cxx -o analysis
: '
Compiles the analysis.cxx file into a binary named "analysis".
This file performs statistical and monthly aggregations from the cleaned dataset.
'

chmod +x run_analysis_script.sh
: '
Gives execution permission to the bash script "run_analysis_script.sh",
ensuring that it can be run as a program.
'

./run_analysis_script.sh
: '
Executes the analysis bash script which:
  - Calls the compiled analysis binary
  - Generates monthly CSV results for each city and year
  - Stores them inside results/
'


#Plot Generation

echo "==> Generating plots..."
cd "$SCRIPT_DIR/plots/"

: '
We move into the plots/ directory where the ROOT plotting scripts are located.

ROOT is a scientific data visualization framework.
Here, we use ROOT macros (like plot_monthly_using_csv_data.C)
to generate monthly rainfall/temperature plots.
'

chmod +x run_root_plots.sh
: '
Makes the plotting bash script executable.
'

./run_root_plots.sh
: '
Runs the plotting script, which:
  - Reads the monthly CSVs from results/
  - Calls ROOT macros to create .png plots
  - Saves them in the figures/ directory
'


#Cleanup
echo "==> Cleaning up temporary CSV files and results folder..."
cd "$SCRIPT_DIR"

: '
Now that the plots have been generated, we clean up intermediate files.
This ensures that only the raw dataset and final figures remain.
'

# Remove all CSV files except the original dataset in datasets/
find . -type f -name "*.csv" ! -path "./datasets/SMHI_pthbv_p_t_1961_2025_daily_4326.csv" -delete
: '
find .                  → search from the current directory downward
-type f                 → match only files (not directories)
-name "*.csv"           → find all CSV files
! -path "./datasets/SMHI_pthbv_p_t_1961_2025_daily_4326.csv" → exclude the original dataset
-delete                 → permanently remove all other CSV files

Effectively removes:
  - Cleaned CSVs
  - Monthly analysis outputs
  - Any other temporary CSVs created during execution
'

# Remove the entire results folder
if [ -d "$SCRIPT_DIR/results" ]; then
  rm -rf "$SCRIPT_DIR/results"
  echo "Deleted folder: $SCRIPT_DIR/results"
fi
: '
Checks if the directory results/ exists.
If it does, rm -rf recursively removes it and all its contents.
This ensures no intermediate analysis data remains.
'


echo "Plots have been generated successfully!"
echo "All intermediate CSVs and the results/ folder have been removed."
