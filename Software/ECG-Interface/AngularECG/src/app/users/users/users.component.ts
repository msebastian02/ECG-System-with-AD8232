import { Component, OnInit, ViewChild, TemplateRef } from '@angular/core';
import { Patient } from 'src/app/Models/Patient';
import { Router } from '@angular/router';
import { NgbModal } from '@ng-bootstrap/ng-bootstrap';
import { ServiceService } from 'src/app/Service/service.service';
import { FormGroup, FormBuilder, FormArray, FormControl, Validators } from '@angular/forms';
import { SampleData } from 'src/app/Models/SampleData';

@Component({
  selector: 'app-users',
  templateUrl: './users.component.html',
  styleUrls: ['./users.component.css']
})
export class UsersComponent implements OnInit {

  @ViewChild('addSamplesModal', {static: true}) targetModal: TemplateRef<any>;
  @ViewChild('newUserModal', {static: true}) newUserModal: TemplateRef<any>;
  patient: Patient;
  users: Patient[];
  toggle: boolean;
  isVisible: boolean;
  isSelected: boolean;
  isEmpty: boolean;
  hideFilter: boolean;
  isAddingUser: boolean;
  isSelectingUser: boolean;
  filtername: string;
  addSamplesForm: FormGroup;
  newUserForm: FormGroup;
  samplesA: SampleData[];

  constructor(
    private modalService: NgbModal,
    private service: ServiceService,
    private router: Router,
    private formBuilder: FormBuilder
  ) {
    this.isVisible = true;
    this.isEmpty = false;
    this.users = [{name: 'Empty'}];
    this.hideFilter = false;
    this.isAddingUser = false;
    this.isSelectingUser = false;
  }
  ngOnInit(): void {
    this.service.getUsers()
    .subscribe((response) => {
      if (response.data.length > 0) {
        this.users = response.data;
        this.isVisible = false;
      }
    }, (error) => {
      console.log('Error in /users: ', error);
    });
  }
  userClick(userName: string) {
    this.isSelectingUser = true;
    this.service.getUserInfo(userName)
    .subscribe((response) => {
      this.patient = {
        name: response.data.name,
        address: response.data.address,
        age: response.data.age,
        email: response.data.email,
        phone: response.data.phone,
        samples: response.data.samples
      };
      this.isEmpty = true;
      this.isSelected = true;
      this.isSelectingUser = false;
    }, (error) => {
      console.log('Error in /userinfo: ', error);
    });
  }
  getDate(date: string): Date {
    if (date !== 'null' && new Date(date).getFullYear() > 1990) {
      const localDate = new Date(date);
      const localTime = localDate.getTime();
      return new Date(localTime);
    }
    return null;
  }
  toGraph(index: number) {
    console.log('Graph index: ', index);
    this.router.navigate(['graph'], {state: {data: {patient: this.patient.name, dataId: this.patient.samples[index].id}}});
  }
  deleteSample(index: number) {
    console.log('Sample to delete: ', this.patient.samples[index]);
    this.service.postDeleteSample(this.patient.samples[index].id + ',' + this.patient.name)
    .subscribe((response) => {
      if (response.data) {
        this.patient.samples.splice(index, 1);
      }
    }, (error) => {
      console.log('Error in /deletesample', error);
    });
  }
  addSample() {
    this.service.getSamples()
    .subscribe((response) => {
      this.samplesA = response.data;
      const formControls = this.samplesA.map(control => new FormControl(false));
      this.addSamplesForm = this.formBuilder.group({
        samples: new FormArray(formControls)
      });
      this.modalService.open(this.targetModal, {
        centered: true,
        backdrop: 'static'
      });
    }, (error) => {
      console.log('Error in /samples: ', error);
    });
  }
  onSubmit(modal) {
    const selectedSamples = this.addSamplesForm.value.samples
    .map((checked, index) => checked ? this.samplesA[index] : null)
    .filter(value => value !== null);
    let samples = '';
    selectedSamples.forEach(sample => {
      samples = samples + sample.id + ',';
    });
    samples = samples + this.patient.name;
    this.service.postNewSamples(samples)
    .subscribe((response) => {
      if (response.data) {
        this.patient.samples.push(selectedSamples);
        modal.dismiss();
      }
    }, (error) => {
      console.log('Error in /newsamples: ', error);
    });
  }
  addUser() {
    this.newUserForm = this.formBuilder.group({
      name: new FormControl('', [
        Validators.pattern(/^[a-zA-Z][ a-zA-Z ]*[a-zA-Z]$/),
        Validators.required
      ]),
      age: new FormControl('', [
        Validators.pattern('[0-9]+'),
        Validators.maxLength(2),
        Validators.required
      ]),
      address: new FormControl('', [
        Validators.pattern(/^[a-zA-Z][ a-zA-Z0-9 ]*[a-zA-Z0-9]$/),
        Validators.required
      ]),
      phone: new FormControl('', [
        Validators.minLength(10),
        Validators.required
      ]),
      email: new FormControl('', [
        Validators.pattern(/^[a-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]+$/),
        Validators.required
      ])
    });
    this.modalService.open(this.newUserModal, {
      centered: true,
      backdrop: 'static'
    });
  }
  get name() { return this.newUserForm.get('name');  }
  get age() { return this.newUserForm.get('age');  }
  get address() { return this.newUserForm.get('address');  }
  get phone() { return this.newUserForm.get('phone');  }
  get email() { return this.newUserForm.get('email');  }
  submitUser(modal) {
    this.isAddingUser = true;
    const userInfo = this.newUserForm.get('name').value + ',' +
    this.newUserForm.get('age').value + ',' +
    this.newUserForm.get('email').value + ',' +
    this.newUserForm.get('phone').value + ',' +
    this.newUserForm.get('address').value;
    this.service.postNewUser(userInfo)
    .subscribe((response) => {
      if (response.data) {
        modal.dismiss();
        this.users.push({
          name: this.newUserForm.get('name').value
        });
      }
    }, (error) => {
      console.log('Error in /newuser: ', error);
    });
  }
  sortClick() {
    this.toggle = !this.toggle;
    return this.users.sort((a, b) => {
      if (a.name > b.name) {return this.toggle ? 1 : -1; }
      if (a.name < b.name) {return this.toggle ? -1 : 1; }
      return 0;
    });
  }
  searchClick() {
    this.hideFilter = !this.hideFilter;
  }
}
