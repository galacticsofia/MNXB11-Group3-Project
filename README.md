# MNXB11-Group3-Project

FalunVSFalsterbo use:
Clean the data by using cleaning_data_Falsterbo.cxx and cleaning_data_Falun.cxx. Launch these by writing "g++ cleaning_data_city.cxx -o name", replace city with the data to be cleaned and the name can be whatever. Then obtain the CSV file by writing and using "./name" in the terminal. Both Falun and Falsterbo are required for this code. Repeat the step used for cleaning_data but for FalunVSFalsterbo.cxx instead. Then get the CSV file for that in the same way "./name". Now go in to ROOT and write ".L FalunVSFalsterboPlot.C", then obtain the plot by writing  "PlotTemperatureDifference();".

Warmest_Coldest use:
Clean data using cleaning_data_Uppsala.cxx by writing g++ cleaning_data_Uppsala.cxx -o name and after that ./name in the terminal.
Create csv with coldes and warmest days of each year using warmest_caldest.cxx by writing g++ warmest_coldest.cxx -o name and after that ./name in the terminal.
Create plot in root by writing .L warmest_plot.cxx and then plot_results();

Rain_analysis implementation : 

```bash
# Clone the repository
git clone https://github.com/Ossian-Malmborg/MNXB11-Group3-Project
cd MNXB11-Group3-Project/
# Create Graphical Plots
chmod +x rain_analysis/run_all.sh
./rain_analysis/run_all.sh

