import { Component, OnInit } from '@angular/core';
import { Router } from '@angular/router';
import { ServiceService } from 'src/app/Service/service.service';

@Component({
  selector: 'app-upload',
  templateUrl: './upload.component.html',
  styleUrls: ['./upload.component.css']
})
export class UploadComponent implements OnInit {

  isVisible: boolean;
  message: string;
  path: string;

  constructor(
    private service: ServiceService,
    private router: Router
    ) { }

  ngOnInit(): void {
    this.isVisible = false;
    this.path = 'Upload your file';
  }
  onFileChange(files: any) {
    const file: File = files[0];
    let fileContent: any;
    this.path = file.name;
    if (this.isTXTfile(file.name)) {
      const reader = new FileReader();
      this.isVisible = true;
      reader.onloadend = (e) => {
        fileContent = reader.result;
        this.service.postDataFile(file.name + ',' + fileContent)
        .subscribe((response) => {
          if (response.status) {
            this.router.navigate(['graph'], {state: {data: {dataId: response.id.name}}});
          }
        }, (error) => {
          console.log('Error in /datafile:', error);
        });
     };
      reader.readAsText(file);
    } else {
      this.path = 'Upload your file';
    }
  }
  isTXTfile(csv: string): boolean {
    let isTXT = false;
    if (csv.endsWith('.txt')) {
      this.message = 'Loading graph...';
      isTXT = true;
    } else {
      this.message = 'Please upload a file of type txt';
    }
    return isTXT;
  }

}
