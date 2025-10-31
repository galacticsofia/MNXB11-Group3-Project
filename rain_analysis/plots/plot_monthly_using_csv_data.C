#include <TCanvas.h>
#include <TH1F.h> // histogram
#include <TGraph.h>
#include <TLegend.h>
#include <TGaxis.h> // secondary axis
#include <TAxis.h>
#include <TStyle.h>
#include <TMath.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>



std::vector<std::string> split_csv(const std::string& s){
  std::vector<std::string> v;
  std::string current;
  for(char c: s){
     if(c==','){
       v.push_back(current);
       current.clear();
      } 
      else current.push_back(c);
    }
  v.push_back(current); return v;
}

void plot_monthly_using_csv_data(const char* monthly_csv,const char* station="A",int year=1961){
  // we first read csv file produced by analysis.cxx
  // header: month,total_rain_mm,monthly_tmax_C,monthly_tmin_C,rainy_days
  
  std::ifstream f(monthly_csv);
  if(!f.is_open()){ printf("Cannot open %s\n", monthly_csv); return; } // check incase file does not open

  std::string line;
  if(!std::getline(f, line)){ printf("Empty file: %s\n", monthly_csv); return; } //  check in case file is empty

  double rain[12]={0}, tmax[12]={0}, tmin[12]={0}, days[12]={0};


  while (std::getline(f, line)) {
    if (line.empty()) continue;

    auto cols = split_csv(line);
    if ((int)cols.size() < 5) continue;

    int m = std::stoi(cols[0]);
    if (m < 1 || m > 12) continue;

    // write directly into the arrays (handles empty cells too)
    rain[m-1] = cols[1].empty() ? 0.0 : std::stod(cols[1]);
    tmax[m-1] = cols[2].empty() ? NAN  : std::stod(cols[2]);
    tmin[m-1] = cols[3].empty() ? NAN  : std::stod(cols[3]);
    days[m-1] = cols[4].empty() ? 0.0 : std::stod(cols[4]);

    /*
    // Example: for a CSV line like "3,58.2,11.5,2.1,5"
    // split_csv() → ["3","58.2","11.5","2.1","5"]
    // m = 3 (March), so data is stored at index 2 (m-1):
    //   rain[2] = 58.2
    //   tmax[2] = 11.5
    //   tmin[2] = 2.1
    //   days[2] = 5
    // If any field is empty (e.g. ""), it's replaced with 0.0 or NAN.

    */

  }



  f.close();

  // Canvas 
  // new TCanvas(...) returns a pointer to a heap-allocated TCanvas
  //   c          (pointer to canvas)
  //   ↓
  // ->SetGrid  (member function call on the canvas via pointer)
  auto *c = new TCanvas("c","Monthly summary",1000,650);
  gPad->SetLeftMargin(0.10);
  gPad->SetRightMargin(0.30);   // space for right-axis & legend outside
  gPad->SetBottomMargin(0.12);
  gPad->SetTopMargin(0.12);
  c->SetGrid();

  // rainfall as bar chart with month labels 
  double maxRain = 1.0;
  for(int i=0;i<12;++i) maxRain = std::max(maxRain, rain[i]);

  // new TH1F(...) → pointer to histogram
  // hRain        (pointer to histogram)
  //   ↓
  // ->SetBinContent / ->Draw / ->GetXaxis are member calls via pointer
  
  TH1F *hRain = new TH1F("hRain",Form("Monthly summary (%s, %d);Month;Rainfall (mm)", station, year),12, 0.5, 12.5); // 12 bins (for 12 months) and Bin edges: [0.5, 12.5]

  for(int m=1;m<=12;++m) hRain->SetBinContent(m, rain[m-1]);


  //We label the bins of the bar chart corresonding to the months and adjust : Colors, bar width, offsets, axis label size.
  // mnames is an array of 13 pointers to const char (C string literals live in static storage)
  
  const char* mnames[13]={"","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
  // array of 13 pointers to constant character
  
  
  // hRain             (TH1F* pointer)
  //   ↓
  // ->GetXaxis()      (returns TAxis* pointer to internal x-axis)
  //   ↓
  // ->SetBinLabel()   (sets label on that axis)
  for(int m=1;m<=12;++m) hRain->GetXaxis()->SetBinLabel(m, mnames[m]);

  hRain->SetMaximum(maxRain*1.20);
  hRain->SetMinimum(0);
  hRain->SetFillColor(kGreen+2);
  hRain->SetLineColor(kGreen+3);
  hRain->SetBarWidth(0.75);
  hRain->SetBarOffset(0.12);
  hRain->SetStats(0);

  // labelling
  // hRain->GetXaxis() (TAxis* pointer)
  //   ↓
  // ->LabelsOption / ->SetLabelSize (member calls on TAxis via pointer)
  
  hRain->GetXaxis()->LabelsOption("h");     // horizontal month names
  hRain->GetXaxis()->SetLabelSize(0.045);
  hRain->GetYaxis()->SetLabelSize(0.045);
  // hRain (TH1F*) ->Draw draws the histogram
  hRain->Draw("BAR");

  // right axis (shared by temps & rainy-days) 
  const double leftMax = hRain->GetMaximum();
  double maxT=-1e9, minT=1e9, maxDays=0;
  for(int i=0;i<12;++i){
    maxT   = std::max(maxT, tmax[i]);
    minT   = std::min(minT, tmin[i]);
    maxDays= std::max(maxDays, days[i]);
  }
  const double rightMin = std::floor(std::min(minT - 2.0, 0.0));
  const double rightMax = std::ceil (std::max(maxT + 2.0, maxDays + 2.0));


  // new TGaxis(...) → pointer to axis object drawn on the pad
  // rightAxis   (pointer to TGaxis)
  //    ↓
  //  ->SetTitle / ->Draw are member calls via pointer
  
  TGaxis *rightAxis = new TGaxis(12.5, 0, 12.5, leftMax, rightMin, rightMax, 510, "+L");
  rightAxis->SetTitle("Temperature (^{o}C) and Rainy days / month");
  rightAxis->SetTitleOffset(1.1);
  rightAxis->SetLabelSize(0.045);
  rightAxis->Draw();

  // mapToLeft is a capturing lambda (value function,)
 
  auto mapToLeft = [&](double y){ return (y - rightMin) * (leftMax) / (rightMax - rightMin); };

  // Overlay Tmax/Tmin/Days as lines
  double x[12], yTmax[12], yTmin[12], yDays[12];
  for(int i=0;i<12;++i){
    x[i]     = i+1;
    yTmax[i] = mapToLeft(tmax[i]);
    yTmin[i] = mapToLeft(tmin[i]);
    yDays[i] = mapToLeft(days[i]);
  }

  // new TGraph(...) → TGraph*; then member calls via pointer
  
  // gTmax (pointer) ->SetLineColor/Width ->Draw
  TGraph *gTmax = new TGraph(12, x, yTmax);
  gTmax->SetLineColor(kBlue+1);
  gTmax->SetLineWidth(2);
  gTmax->Draw("L SAME");

  // gTmin (pointer) ->...
  TGraph *gTmin = new TGraph(12, x, yTmin);
  gTmin->SetLineColor(kRed+1);
  gTmin->SetLineWidth(2);
  gTmin->Draw("L SAME");
  
  // gDays (pointer) ->...
  TGraph *gDays = new TGraph(12, x, yDays);
  gDays->SetLineColor(kViolet+2);
  gDays->SetLineWidth(2);
  gDays->SetLineStyle(2);
  gDays->Draw("L SAME");

  // legend OUTSIDE the frame (right margin)

  // new TLegend(...) → pointer to legend
  // leg (pointer) ->AddEntry / ->Draw
  TLegend *leg = new TLegend(0.80, 0.72, 0.98, 0.95, "", "NDC");
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(hRain, "Monthly rainfall (mm)", "f"); // passes TH1F* pointer to legend for styling
  leg->AddEntry(gTmax, "Monthly max temp", "l");      // passes TGraph* pointers
  leg->AddEntry(gTmin, "Monthly min temp", "l");
  leg->AddEntry(gDays, "Rainy days / month", "l");
  leg->Draw();

  // gPad is a global pointer to the current pad
  // gPad (TPad*) ->RedrawAxis / ->Update
  gPad->RedrawAxis();
  gPad->Update();

    // Form(...) returns const char*; c (TCanvas*) ->SaveAs writes the file
  // ---------- save (your script runs from repo root, so 'figures/' is correct) ----------
  c->SaveAs(Form("figures/monthly_bar_%s_%d.png", station, year));
}
