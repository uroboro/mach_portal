//
//  ViewController.swift
//  mach_portal
//
//  Created by Ian Beer on 11/27/16.
//  Copyright © 2016 Ian Beer. All rights reserved.
//

import UIKit

class ViewController: UIViewController {
	
	@IBOutlet weak var console: ConsoleView!
	
	var hostname: UITextField?
	var port: UITextField?
	
	let defaultHost: String = "localhost"
	let defaultPort: Int = 4141

    override func viewDidLoad() {
        super.viewDidLoad()
        DispatchQueue.main.async(execute: { () -> Void in
          jb_go();
		})
		
		// set up scrollview for scrolling when keyboard pops up
		NotificationCenter.default.addObserver(self, selector: #selector(keyboardWillShow), name: .UIKeyboardWillShow, object: nil)
		NotificationCenter.default.addObserver(self, selector: #selector(keyboardWillHide), name: .UIKeyboardWillHide, object: nil)
		
	}
	
	override func viewDidAppear(_ animated: Bool) {
		
		// welcome message
		console.log("nc-client for iOS by vgmoose\nCC BY-NC-SA 4.0 license\n")
		
		// disable spell checking (messes up some commands)
		console.autocorrectionType = UITextAutocorrectionType.no;
		
		var alert = UIAlertController(title: "netcat client", message: "Enter hostname and port to connect. Leave blank and press OK for defaults.", preferredStyle: UIAlertControllerStyle.alert)
		//
		
		func hostPrompt(textField: UITextField!) {
			// add the text field and make the result global
			textField.placeholder = "Hostname (default: \(defaultHost))"
			
			// load old value if it exists
			if let hostname = self.hostname {
				textField.text = hostname.text!
			}
			
			hostname = textField
		}
		
		func portPrompt(textField: UITextField!) {
			// add the text field and make the result global
			textField.placeholder = "Port (default: \(defaultPort))"
			
			// load old value if it exists
			if let port = self.port {
				textField.text = port.text!
			}
			
			port = textField
		}
		
		alert.addTextField(configurationHandler: hostPrompt)
		alert.addTextField(configurationHandler: portPrompt)
		alert.addAction(UIAlertAction(title: "OK", style: .default, handler:{ (alertAction:UIAlertAction!) in self.startNC()
		}))
		
		self.present(alert, animated: true)
		
	}
	
	override func didReceiveMemoryWarning() {
		super.didReceiveMemoryWarning()
		// Dispose of any resources that can be recreated.
	}
	
	func startNC() {
		
		// get input ports from alert textfields, or defaults
		let host = (hostname!.text?.isEmpty)! ? hostname!.text! : defaultHost
		let port = Int(self.port!.text!) ?? defaultPort
		
		// run on a background thread
		DispatchQueue.global(qos: .userInitiated).async {
			
			// connect to remote
			self.console.connect(host: host, port: port)
		}
	}
	
	// MARK: UITextFieldDelegate
	
	func textFieldShouldReturn(textField: UITextField) -> Bool {
		textField.resignFirstResponder()
		return true
	}
	
	func keyboardWillShow(notification: NSNotification) {
		
		var userInfo = notification.userInfo!
		var keyboardFrame = (userInfo[UIKeyboardFrameBeginUserInfoKey] as! NSValue).cgRectValue
		keyboardFrame = self.view.convert(keyboardFrame, from: nil)
		
		var contentInset = self.console.contentInset
		contentInset.bottom = keyboardFrame.size.height
		self.console.contentInset = contentInset
	}
	
	func keyboardWillHide(notification: NSNotification) {
		
		self.console.contentInset = UIEdgeInsets.zero
	}


}

