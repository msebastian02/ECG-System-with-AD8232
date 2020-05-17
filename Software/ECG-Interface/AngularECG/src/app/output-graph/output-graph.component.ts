import { Component, OnInit } from '@angular/core';
import { HttpClient, HttpErrorResponse } from '@angular/common/http';
import { interval, Subscription } from 'rxjs';
import { ServiceService } from '../Service/service.service';
import * as Highcharts from 'highcharts';

declare var require: any;

const Boost = require('highcharts/modules/boost');
const noData = require('highcharts/modules/no-data-to-display');
const More = require('highcharts/highcharts-more');
const Export = require('highcharts/modules/exporting');
const ExportData = require('highcharts/modules/export-data');
const Stock = require('highcharts/modules/stock');

Stock(Highcharts);
Boost(Highcharts);
noData(Highcharts);
More(Highcharts);
Export(Highcharts);
ExportData(Highcharts);

@Component({
  selector: 'app-output-graph',
  templateUrl: './output-graph.component.html',
  styleUrls: ['./output-graph.component.css']
})

export class OutputGraphComponent implements OnInit {
  public options: any = {
    title: {
      text: ''
    },
    subtitle: {
        text: 'Source: JSMA-CMD'
    },
    yAxis: {
        title: {
            text: 'Voltage'
        },
        min: 0,
        max: 4,
    },
    xAxis: {
      title: {
          text: 'Samples number'
      },
      min: 0,
      max: 500,
      scrollbar: {
          enabled: true
      }
    },
    legend: {
        layout: 'vertical',
        align: 'right',
        verticalAlign: 'middle'
    },
    plotOptions: {
        series: {
            label: {
                connectorAllowed: false
            },
            pointStart: 1
        }
    },
    series: [{
        name: 'ECGData',
        data: []
    }],
    responsive: {
        rules: [{
            condition: {
                maxWidth: 500
            },
            chartOptions: {
                legend: {
                    layout: 'vertical',
                    align: 'center',
                    verticalAlign: 'bottom'
                }
            }
        }]
    }
  };

  subscription: Subscription;
  constructor( private service: ServiceService, private http: HttpClient) { }

  ngOnInit() {
    Highcharts.chart('container', this.options);
    console.log('In Graph: OnInit!', history.state.data);
    if (history.state.data !== undefined) {
      if (history.state.data.patient === undefined) {
        this.service.getDataFile(history.state.data.dataId)
        .subscribe((response) => {
          console.log(response.data);
          const data1 = response.data.split(',');
          let i;
          const datosnum = [0];
          for (i = 0; i <= data1.length; i++) {
              datosnum[i] = Number.parseFloat(data1[i]);
          }
          this.options.series[0].data = datosnum;
          Highcharts.chart('container', this.options);
        }, (error) => {
          console.log('Error in /datafileid: ', error);
        });
      } else {
        this.service.getData(history.state.data.patient + ',' + history.state.data.dataId)
        .subscribe((response) => {
          console.log(response.data);
          const data1 = response.data.split(',');
          let i;
          const datosnum = [0];
          for (i = 0; i <= data1.length; i++) {
              datosnum[i] = Number.parseFloat(data1[i]);
          }
          this.options.series[0].data = datosnum;
          Highcharts.chart('container', this.options);
        }, (error) => {
          console.log('Error in /data2graph: ', error);
        });
      }
    }
  }

  ObtainData(SampleId: string) {
    const identificador = SampleId;
    fetch('http://127.0.0.1:5000/?id=' + identificador)
    .then((response) => {
        return response.json();
    })
    .then((myJson) => {
      const data = myJson.Samples;
      console.log(myJson);
      const data1 = data.split(',');
      let i;
      const datosnum = [0];
      for (i = 0; i <= data1.length; i++) {
          datosnum[i] = Number.parseFloat(data1[i]);
      }
      this.options.series[0].data = datosnum;
      Highcharts.chart('container', this.options);
      })
      .catch((err: HttpErrorResponse) => {
        alert('Ingrese un ID valido');
      });
    }
}
