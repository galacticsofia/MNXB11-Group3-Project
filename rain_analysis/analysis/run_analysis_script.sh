#!/usr/bin/env bash


ROOT_ANALYSIS="$(cd "$(dirname "$0")" && pwd)"
ROOT_PROJECT="$(cd "$ROOT_ANALYSIS/.." && pwd)"
: '
"$0" refers to our current script file: run_analysis.sh
dirname "$0" helps us retrieve the parent directory: the analysis/ folder.
cd ... && pwd gives the absolute path to that folder.
Then, moving one directory up (..) gives the project root
'

DATA_CLEAN="$ROOT_PROJECT/data_clean/Rain_temperature_cleaned.csv"
RESULTS_DIR="$ROOT_PROJECT/results"
SRC_ANALYSIS="$ROOT_PROJECT/analysis/analysis.cxx"
EXE_ANALYSIS="$ROOT_PROJECT/analysis/analysis"

: '
$DATA_CLEAN :refers to the input dataset that is the cleaned dataset
$RESULTS_DIR :folder to save our results post analysis
$SRC_ANALYSIS :location of our analysis.cxx file
$EXE_ANALYSIS : the compiled object binary file of analysis.cxx
'

mkdir -p "$RESULTS_DIR" 
: '
makes a directory to store our results in case it does not exist
-p makes sure if the file exists there is not error
'

echo "Compiling $SRC_ANALYSIS -> $EXE_ANALYSIS"
g++ "$SRC_ANALYSIS" -o "$EXE_ANALYSIS"
: '
We print a message so that  the user know the bash file is running and we complile the relavant file.
'

declare -A CITY=([A]="Lund" [B]="Uppsala") 
: '
puts A as Lund and B as Uppsala, this helps us but making it convinient to indentify files
'

echo "Running analyses..."
for year in 1961 2024; do
  for station in A B; do    
    city="${CITY[$station]}"
    out="$RESULTS_DIR/monthly_${city}_${year}.csv"
    echo "  $city ($station), $year"
    "$EXE_ANALYSIS" "$DATA_CLEAN" "$year" "$station" "$out"
  done
done

echo "Done. CSVs in: $RESULTS_DIR"
