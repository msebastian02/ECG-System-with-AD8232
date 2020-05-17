
import { Component, OnInit, ViewChild, TemplateRef } from '@angular/core';
import { NgbModal } from '@ng-bootstrap/ng-bootstrap';
import { FormGroup, FormBuilder, FormControl, Validators } from '@angular/forms';
import { AdminUser } from 'src/app/Models/AdminUser';
import { ServiceService } from 'src/app/Service/service.service';
@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})
export class LoginComponent implements OnInit {

  @ViewChild('loginUserModal', {static: true}) targetModal: TemplateRef<any>;
  loginInformationForm: FormGroup;
  errorMessage: string;
  isVisible: boolean;

  constructor(
    private service: ServiceService,
    private modalService: NgbModal,
    private formBuilder: FormBuilder) {
      this.isVisible = false;
    }

  ngOnInit() {
    this.loginInformationForm = this.formBuilder.group({
      username: new FormControl('', [
        Validators.pattern('[a-zA-Z]+'),
        Validators.required
      ]),
      password: new FormControl('', [
        Validators.minLength(4),
        Validators.pattern('[0-9]+'),
        Validators.required
      ])
    });
    this.service.getIsLogin()
    .subscribe((response) => {
      if (!response.data) {
        this.modalService.open(this.targetModal, {
          centered: true,
          backdrop: 'static',
          keyboard: false
        });
      }
    }, (error) => {
      console.log('Error of /islogin: ', error);
    });
  }
  onSubmit(modal) {
    this.isVisible = true;
    this.service.getLogin(this.loginInformationForm.get('username').value + ',' + this.loginInformationForm.get('password').value)
    .subscribe((data) => {
      this.isVisible = false;
      if (data.data) {
        modal.dismiss();
        window.location.reload();
      } else {
        this.errorMessage = 'Incorrect username or password';
      }
    }, (error) => {
      this.isVisible = false;
      console.log('Error of /login: ', error);
      this.errorMessage = 'Incorrect username or password';
    });
  }
  get username() { return this.loginInformationForm.get('username');  }
  get password() { return this.loginInformationForm.get('password');  }
}
