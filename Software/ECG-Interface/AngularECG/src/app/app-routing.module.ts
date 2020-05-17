import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { AppComponent } from './app.component';
import { AboutUsComponent } from './about-us/about-us.component';
import { OutputGraphComponent } from './output-graph/output-graph.component';
import { LoginComponent } from './login/login/login.component';
import { UsersComponent } from './users/users/users.component';
import { UploadComponent } from './upload/upload/upload.component';


const routes: Routes = [
  { path: 'graph', component: OutputGraphComponent },
  { path: 'aboutus', component: AboutUsComponent },
  { path: 'login', component: LoginComponent },
  { path: 'home', component: UsersComponent},
  { path: 'upload', component: UploadComponent },
  { path: '**', pathMatch: 'full', redirectTo: 'home'}
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
