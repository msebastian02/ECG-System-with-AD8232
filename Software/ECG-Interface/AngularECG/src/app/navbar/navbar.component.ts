import { Component, OnInit, ViewChild, TemplateRef, OnDestroy, HostListener } from '@angular/core';
import { ServiceService } from '../Service/service.service';
import { NgbModal } from '@ng-bootstrap/ng-bootstrap';

@Component({
  selector: 'app-navbar',
  templateUrl: './navbar.component.html',
  styleUrls: ['./navbar.component.css']
})
export class NavbarComponent implements OnInit, OnDestroy {

  @ViewChild('loginUserModal', {static: true}) targetModal: TemplateRef<any>;
  username: string;

  constructor(
    private modalService: NgbModal,
    private service: ServiceService) {
      this.username = '';
    }

  ngOnInit(): void {
    this.service.getIsLogin()
    .subscribe((response) => {
      if (response.data) {
        this.username = response.name;
      }
    }, (error) => {
      console.log('Error in /islogin: ', error);
    });
  }
  ngOnDestroy(): void {
    this.modalService.open(this.targetModal, {
      centered: true,
      backdrop: 'static'
    });
    this.signOut();
  }
  signOut() {
    this.service.getSignOut(this.username)
    .subscribe((response) => {
      if (response.data) {
        this.username = '';
        window.location.reload();
      }
    }, (error) => {
      console.log('Error in /logout: ', error);
    });
  }



}
