Int_t n = MeasurementData->Draw("ResistanceCh9:Temperature","ResistanceCh10<1e8&&ResistanceCh10>0&&ResistanceCh9<1e8&&ResistanceCh9>0","goff"); TGraph *graph = new TGraph(n, MeasurementData->GetV2(), MeasurementData->GetV1()); graph->SetMarkerStyle(22); graph->Draw("APL");
c1->SaveAs("JYCanvas.root");
     
