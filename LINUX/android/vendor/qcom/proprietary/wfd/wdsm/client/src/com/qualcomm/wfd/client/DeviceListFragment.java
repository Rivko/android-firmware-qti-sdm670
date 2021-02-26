/*
 * Copyright (c) 2012-2014, 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution, Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.wfd.client;

import android.app.AlertDialog;
import android.app.ListFragment;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.net.wifi.WpsInfo;
import android.net.wifi.p2p.WifiP2pConfig;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.LayoutInflater;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.wfd.WfdEnums;
import com.qualcomm.wfd.client.net.ConnectionInfoIF;
import com.qualcomm.wfd.client.net.LanDevice;
import com.qualcomm.wfd.client.net.NetDeviceIF;
import com.qualcomm.wfd.client.net.NetManagerIF;
import com.qualcomm.wfd.client.net.ResultListener;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

/**
 * A ListFragment that displays available peers on discovery and requests the
 * parent activity to handle user interaction events
 */
public class DeviceListFragment extends ListFragment {
	private static final String TAG = "C.DeviceListFragment";
	private static final int WLAN_EVENT_P2P_PEER_LIST_FINISH = 0;
	public static List<NetDeviceIF> peers = new ArrayList<NetDeviceIF>();
	private Boolean isFilterOn = false;
	ProgressDialog progressDialog = null;
	ProgressDialog connectProgressDialog = null;
	View mContentView = null;
	private Button toggleButton;
	private FrameLayout deviceDetailsFrameLayout;
	private EventHandler eventHandler;
	private NetDeviceIF tempDevice;
	public static final int connectionInfoAvailable = 1; // msg.obj = WifiP2pInfo
    private HashMap<String, NetDeviceIF> mConnectedDevices = new HashMap<String, NetDeviceIF>();
    private WFDClientActivity activity;

	private Whisperer whisperer;
	public NetDeviceIF connectedDevice; // only for single mode

    @Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);
        activity = (WFDClientActivity)getActivity();
		this.setListAdapter(new WiFiPeerListAdapter(activity,
				R.layout.row_devices, peers));
		eventHandler = new EventHandler();
		whisperer = Whisperer.getInstance();
	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		mContentView = inflater.inflate(R.layout.device_list, null);

		eventHandler = new EventHandler();

		return mContentView;
	}

	/**
	 * Array adapter for ListFragment that maintains QCWifiP2pDevice list.
	 */
	private class WiFiPeerListAdapter extends ArrayAdapter<NetDeviceIF> {

		private List<NetDeviceIF> items;

		/**
		 * @param context
		 * @param textViewResourceId
		 * @param objects
		 */
		public WiFiPeerListAdapter(Context context, int textViewResourceId,
				List<NetDeviceIF> objects) {
			super(context, textViewResourceId, objects);
			items = objects;
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			Log.d(TAG, "getView called with position: " + position);
			final NetDeviceIF device = items.get(position);
			/*
			View v = convertView;
			if (v != null && !device.isStatusConnected() && (Boolean)v.getTag(R.string.connected)) {
				// do not reuse view in connected state, otherwise UI may show wrong state
				v = null;
			}
			*/
			View v = null;
			if (v == null) {
				LayoutInflater vi = (LayoutInflater) getActivity()
						.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
				v = vi.inflate(R.layout.row_devices, null);
				v.setTag(R.string.position, position);
				v.setTag(R.string.expanded, false);
				v.setTag(R.string.connected, false);
			}
			v.setTag(R.string.position, position);

			if (device != null) {
				TextView top = (TextView) v.findViewById(R.id.device_name);
				TextView middle = (TextView) v
						.findViewById(R.id.device_mac_address);
				TextView bottom = (TextView) v
						.findViewById(R.id.device_details);
				ImageView signalLevel = (ImageView)v.findViewById(R.id.signal_level);
				if (top != null) {
					top.setText(device.getName());
				}
				if (middle != null) {
					middle.setText(device.getAddress());
				}
				if (bottom != null) {
					bottom.setText(device.getStatusString());
				}

				if (device.getNetType() == WfdEnums.NetType.WIGIG_P2P) {
					signalLevel.setVisibility(View.VISIBLE);
				} else {
					signalLevel.setVisibility(View.INVISIBLE);
				}

				if (device.getNetType() == WfdEnums.NetType.LAN && middle != null) {
					middle.setText(((LanDevice)device).getServiceType());
				}

				if (device.isStatusConnected()) {
					v.setTag(R.string.connected, true);
					Log.d(TAG, "getView called on connected device: " + device.getName());
					if (connectProgressDialog != null && connectProgressDialog.isShowing()) {
						connectProgressDialog.dismiss();
						Log.d(TAG, "ConnectProgressDialog.dismiss() 1");
					}
					//connectedDevice = device;
					setConnectedDevice(device);
					deviceDetailsFrameLayout = (FrameLayout) v
							.findViewById(R.id.fl_device_detail);
					LinearLayout linLayoutLevel1 = (LinearLayout) deviceDetailsFrameLayout
							.getParent();
					linLayoutLevel1.setTag(R.string.expanded, true);
					deviceDetailsFrameLayout.setVisibility(View.VISIBLE);
					TextView deviceinfoTextView = (TextView) deviceDetailsFrameLayout
							.findViewById(R.id.tv_device_info);
					deviceinfoTextView.setText(device.toString()+ device.getWfdInfoString());
					if (/*netManager.getLocalDevice().hasConnectionInfo() && */ !whisperer.isMultiSinkMode()) {
					NetManagerIF netManager = device.getNetTypeManager();
					ConnectionInfoIF info = null;
						if (netManager != null) {
							info = netManager.getLocalConnectionInfo();
						}
						if (info != null) {
							onConnectionInfoAvailable(device.getNetType(), info);
						} else {
							Log.w(TAG, "device is connected but connection info is null");
						}
                    }
				} else if (device.isStatusFailed()) {
					Log.d(TAG, "getView called on device that failed to connect");
					if (connectProgressDialog != null && connectProgressDialog.isShowing()) {
						connectProgressDialog.dismiss();
						Log.d(TAG, "ConnectProgressDialog.dismiss() 2");
					}
				}
			}
			return v;
		}
	}

	/**
	 * Initiate a connection with the peer.
	 */
	@Override
	public void onListItemClick(ListView l, View v, int position, long id) {
		Log.d(TAG, "onListItemClick called on position: " + position);

		NetDeviceIF device = (NetDeviceIF) getListAdapter().getItem(position);

		// Expand deviceDetail if not already
		FrameLayout clickedDeviceDetailsFrameLayout = (FrameLayout) v
				.findViewById(R.id.fl_device_detail);
		LinearLayout linLayoutLevel1 = (LinearLayout) clickedDeviceDetailsFrameLayout
				.getParent();
		boolean expanded = (Boolean) linLayoutLevel1.getTag(R.string.expanded);
		if (!expanded) {
			Log.d(TAG, "onListItemClick called on list item that wasn't expanded"+ device.getWfdInfoString());
			linLayoutLevel1.setTag(R.string.expanded, true);
			clickedDeviceDetailsFrameLayout.setVisibility(View.VISIBLE);
			TextView deviceinfoTextView = (TextView) clickedDeviceDetailsFrameLayout
					.findViewById(R.id.tv_device_info);
			deviceinfoTextView.setText(device.toString() + device.getWfdInfoString());
		} else {
			Log.d(TAG, "onListItemClick called on list item that was already expanded");
			linLayoutLevel1.setTag(R.string.expanded, false);
			clickedDeviceDetailsFrameLayout.setVisibility(View.GONE);
		}
	}

	public void handleConnectClick(View v) {
		Log.d(TAG, "handleConnectClick called");
		LinearLayout linLayoutLevel4 = (LinearLayout) v.getParent();
		LinearLayout linLayoutLevel3 = (LinearLayout) linLayoutLevel4
				.getParent();
		deviceDetailsFrameLayout = (FrameLayout) linLayoutLevel3.getParent();
		LinearLayout linLayoutLevel1 = (LinearLayout) deviceDetailsFrameLayout
				.getParent();
		int position = (Integer) linLayoutLevel1.getTag(R.string.position);
		final NetDeviceIF device = ((WiFiPeerListAdapter) getListAdapter())
				.getItem(position);

		if (!device.supportWfd()) {
	        Log.d(TAG, "handleConnectClick- device.WfdInfo == null");
			AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
			builder.setTitle("Selected device is not a WFD Capable Device")
				   .setMessage("Would you like to select another device instead?")
			       .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
			           public void onClick(DialogInterface dialog, int id) {
			        	   return;
			           }
			       })
			       .setNegativeButton("No", new DialogInterface.OnClickListener() {
					   public void onClick(DialogInterface dialog, int id) {
						   handleConnectClickHelper(device);
					   }
				   });
			AlertDialog alert = builder.create();
			alert.show();
		} else if (!device.isWfdSessionAvailable()){
		    Log.d(TAG, "handleConnectClick- device.wfdInfo.isSessionAvailable() == false");
		    AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
            builder.setTitle("Selected device is not currently available for a WFD session")
                   .setMessage("Would you like to select another device instead?")
                   .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                       public void onClick(DialogInterface dialog, int id) {
                           return;
                       }
                   })
                   .setNegativeButton("No", new DialogInterface.OnClickListener() {
                       public void onClick(DialogInterface dialog, int id) {
                            handleConnectClickHelper(device);
                       }
                   });
            AlertDialog alert = builder.create();
            alert.show();
        } else if (ServiceUtil.getmServiceAlreadyBound()){
            try{
                if(activity.getCurrentSession() != null){
                    Log.d(TAG,"Attempt to click Connect button when session state is valid");
                    AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
                    builder.setTitle("Connect Button clicked unexpectedly")
                           .setMessage("Please teardown current session before attempting to connect to another device")
                           .setNeutralButton("Ok", new DialogInterface.OnClickListener() {
                               public void onClick(DialogInterface dialog, int id) {
                                   return;
                               }
                            });
                    AlertDialog alert = builder.create();
                    alert.show();
                } else {
                    Log.d(TAG,"Session State is invalid. Processing connect click");
                    Log.d(TAG, "handleConnectClick- call handleConnectClickHelper");
                    handleConnectClickHelper(device);
                }
            }catch (Exception e) {
                e.printStackTrace();
            }
        }
        else {
            Log.d(TAG, "handleConnectClick- call handleConnectClickHelper");
			handleConnectClickHelper(device);
		}
	}

	private void handleConnectClickHelper(NetDeviceIF device) {
		if (progressDialog != null && progressDialog.isShowing()) {
			progressDialog.dismiss();
		}
		if (device.getNetType() == WfdEnums.NetType.LAN) {
			connectProgressDialog = ProgressDialog.show(getActivity(),
					"Press back to cancel", "Connecting to :"
							+ device.getName(), true, true);
		} else {
			connectProgressDialog = ProgressDialog.show(getActivity(),
					"Press back to cancel", "Connecting to :"
							+ device.getAddress(), true, true);
		}
		connect(device);
	}

	public void handleConnectOptionsClick(View v) {
		Log.d(TAG, "handleConnectOptionsClick called");
		LinearLayout linLayoutLevel4 = (LinearLayout) v.getParent();
		LinearLayout linLayoutLevel3 = (LinearLayout) linLayoutLevel4
				.getParent();
		deviceDetailsFrameLayout = (FrameLayout) linLayoutLevel3.getParent();
		LinearLayout linLayoutLevel1 = (LinearLayout) deviceDetailsFrameLayout
				.getParent();
		int position = (Integer) linLayoutLevel1.getTag(R.string.position);
		tempDevice = ((WiFiPeerListAdapter) getListAdapter()).getItem(position);

		registerForContextMenu(v);
		getActivity().openContextMenu(v);
	}

	public void handleStartClientClick(View v) {
		Log.d(TAG, "handleStartClientClick called");
        if (!whisperer.isMultiSinkMode()) {
            LinearLayout linLayoutLevel4 = (LinearLayout) v.getParent();
            LinearLayout linLayoutLevel3 = (LinearLayout) linLayoutLevel4
                    .getParent();
            FrameLayout frameLayoutLevel2 = (FrameLayout) linLayoutLevel3
                    .getParent();
            LinearLayout linLayoutLevel1 = (LinearLayout) frameLayoutLevel2
                    .getParent();
            int position = (Integer) linLayoutLevel1.getTag(R.string.position);
            final NetDeviceIF peerDevice = ((WiFiPeerListAdapter) getListAdapter())
                    .getItem(position);
        }
        //final WifiP2pDevice peerDevice = connectedDevice;
		final NetDeviceIF peerDevice = connectedDevice;
		Log.d(TAG, "handleStartClientClick: peer macAddress= " + peerDevice.getAddress());
		if (!peerDevice.supportWfd()){// || peerDevice.wfdInfo.isSessionAvailable() == false){
            Log.d(TAG, "handleStartClientClick: peerDevice not available for session");
            AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
            builder.setTitle(peerDevice.getAddress() + " is not currently available for a WFD session")
                    .setMessage(
                            "Disconnect and try again after resetting peer device if it worked earlier"
                                    +"\n"+"Or select another device if peer device is not WFD capable")
                    .setNeutralButton("Ok",
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int id) {
                                    return;
                                }
                            });
            AlertDialog alert = builder.create();
            alert.show();
		} else if (!whisperer.isMultiSinkMode()) {
           if (deviceDetailsFrameLayout != null) {
               deviceDetailsFrameLayout.findViewById(R.id.btn_start_client)
                       .setEnabled(false);
           } else {
               Log.e(TAG, "deviceDetailsFrameLayout is null. Really??");
               return;
           }
        }
        ((DeviceActionListener) getActivity()).startSession(peerDevice);
	}

	public void handleFilterClick() {
		Log.d(TAG, "handleFilterClick called");
		isFilterOn = !isFilterOn;
		((WiFiPeerListAdapter) getListAdapter()).notifyDataSetChanged();
	}

	// update peers list in non-multisink mode
	private void onPeersAvailable(Collection<NetDeviceIF> peers) {
		Log.d(TAG, "onPeersAvailable called");
		if (progressDialog != null && progressDialog.isShowing()) {
			progressDialog.dismiss();
		}

		if (peers == null || peers.size() == 0) {
			Log.d(TAG, "No devices found");
			return;
		}
		((DeviceActionListener) getActivity()).peersReceived();
		Message message = eventHandler
				.obtainMessage(WLAN_EVENT_P2P_PEER_LIST_FINISH);
		message.obj = peers;

		eventHandler.sendMessage(message);
	}

	public void clearPeers() {
		Log.d(TAG, "clearPeers called");
		peers.clear();
		((WiFiPeerListAdapter) getListAdapter()).notifyDataSetChanged();
	}

	/**
     *
     */
	public void onInitiateDiscovery() {
		Log.d(TAG, "onInitiateDiscovery() called");
		if (progressDialog != null && progressDialog.isShowing()) {
			progressDialog.dismiss();
		}
		progressDialog = ProgressDialog.show(getActivity(),
				"Press back to cancel", "finding peers", true, true,
				new DialogInterface.OnCancelListener() {

					@Override
					public void onCancel(DialogInterface dialog) {

					}
				});
	}

	@Override
	public void onCreateContextMenu(ContextMenu menu, View v,
			ContextMenuInfo menuInfo) {
		super.onCreateContextMenu(menu, v, menuInfo);
		MenuInflater menuInflater = getActivity().getMenuInflater();
		menuInflater.inflate(R.menu.connection_menu, menu);
	}

	private void connect(final NetDeviceIF dev) {
		dev.connect(new ResultListener() {
			@Override
			public void onSuccess(Object result) {
				setConnectedDevice(dev);
			}

			@Override
			public void onFailure(Object err) {
				Toast.makeText(activity,
						"Connect failed. Retry.", Toast.LENGTH_SHORT)
						.show();
			}
		});
	}

	@Override
	public boolean onContextItemSelected(MenuItem item) {
		NetDeviceIF deviceInfo = tempDevice;

		String option = (String) item.getTitle();
		Toast.makeText(getActivity(),
				"Connection option " + option + " selected", 1500).show();
		Log.d(TAG, "Connection option " + option + " selected");

		Log.d(TAG, "BEFORE new wifip2pconfig");
		WifiP2pConfig config = new WifiP2pConfig();
		Log.d(TAG, "After new wifip2pconfig");
		config.deviceAddress = tempDevice.getAddress();

		SharedPreferences sharedPrefs = PreferenceManager
				.getDefaultSharedPreferences(getActivity());

		switch (item.getItemId()) {
		case R.id.WPS_PBC:
			config.wps.setup = WpsInfo.PBC;
			if (sharedPrefs.getBoolean(
					"perform_custom_group_owner_intent_value", false) == true) {
				config.groupOwnerIntent = Integer.parseInt(sharedPrefs
						.getString("group_owner_intent_value", "3"));
			}
			Log.d(TAG, "group owner intent value: " + config.groupOwnerIntent);
			break;
		case R.id.WPS_DISPLAY:
			config.wps.setup = WpsInfo.DISPLAY;
			break;
		case R.id.WPS_KEYPAD:
			config.wps.setup = WpsInfo.KEYPAD;
			Log.d(TAG, "config.wps.pin: " + config.wps.pin);
			break;
		default:
			Toast.makeText(getActivity(), "Not a valid selection", 4000);
			Log.d(TAG, "Not a valid selection");
			break;
		}

		Log.d(TAG, "config.wps.setup: " + config.wps.setup);
		if (progressDialog != null && progressDialog.isShowing()) {
			progressDialog.dismiss();
		}
		progressDialog = ProgressDialog.show(getActivity(),
				"Press back to cancel", "Connecting to :"
						+ tempDevice.getAddress(), true, true
				);
		deviceInfo.setConfig(config);
		connect(deviceInfo);
		//((DeviceActionListener) getActivity()).connect(config, WifiP2pNetDevice.tmpWrap(tempDevice));
		return true;
	}


    public void onConnectionInfoAvailable(WfdEnums.NetType netType, ConnectionInfoIF info) {
        Log.d(TAG, "onConnectionInfoAvailable() called: " + netType + ", " + info);
        if (connectProgressDialog != null && connectProgressDialog.isShowing()) {
            connectProgressDialog.dismiss();
        }
        if (!whisperer.isMultiSinkMode()) {
            Message message = eventHandler.obtainMessage(connectionInfoAvailable, netType.ordinal(), 0, info);
            eventHandler.sendMessage(message);
        } else {
            Log.d(TAG,"Ignoring connection info from Wi-Fi Direct");
        }
    }

	public void setConnectedDevice(NetDeviceIF device) {
		Log.d(TAG, "setConnectedDevice() called");
		connectedDevice = device;
	}

	// called from event handler
	public void onPeersChanged(Collection<NetDeviceIF> peers) {
		boolean manualFindOnly = whisperer.getSharedPrefs().getBoolean("perform_manual_find_only", false);
            if (whisperer.isMultiSinkMode()) {
                onMultiSinkPeersChanged(peers);
            } else if (whisperer.isDiscoveryRequested() || !manualFindOnly) {
				//onPeersChanged(peers);
				onPeersAvailable(peers);
            }
	}

	// called from event handler
	public void onConnectedPeersChanged(Collection<NetDeviceIF> connectedPeers) {
		if (whisperer.isMultiSinkMode()) {
			setConnectedDevice(connectedPeers);
		}
	}

	// update connected devices in multisink mode
    private void setConnectedDevice(Collection<NetDeviceIF> devices) {
        if (!devices.isEmpty()) {
            //int localDeviceType = ((DeviceActionListener)getActivity()).getLocalDeviceType().getCode();
			//WfdEnums.WFDDeviceType localDeviceType = netManager.getLocalDevice().getWfdType();
			WfdEnums.WFDDeviceType localDeviceType = whisperer.getLocalDeviceType();
            for(NetDeviceIF device: devices) {
                if(device.supportWfd()) {
                    Log.d(TAG,"For " + device.getName() + " wfdinfo is " + device.getWfdInfoString());
                    // We need to ensure a few things before kicking off a session:
                    // 1. It's a different device type than the local device
                    // 2. It's WFD IEs indicate its available for a session
                    // 3. It's not already part of the connected devices list
					if (device.isWfdSessionAvailable() &&
							//convertP2pDevToWfd(device.wfdInfo.getDeviceType()).getCode() != localDeviceType
							device.getWfdType() != localDeviceType
							&& (mConnectedDevices.get(device.getAddress()) == null)){
						connectedDevice = device;
                        mConnectedDevices.put(device.getAddress(), device);
                        Log.d(TAG, "setConnectedDevice() set connected device to " + device.getName());
                        handleStartClientClick(null);
                        return;
                    }
                }
            }
            //delete devices from list if they are not in group
            Iterator connectedDeviceItr = mConnectedDevices.entrySet().iterator();
            while (connectedDeviceItr.hasNext()) {
                Map.Entry entry = (Map.Entry) connectedDeviceItr.next();
                String deviceAdd = (String) entry.getKey();
                NetDeviceIF delDevice = (NetDeviceIF)entry.getValue();
                if(!devices.contains(delDevice)) {
                    //This P2P device was in the connected device list but isn't
                    //anymore in the new Peer list, so it's gone, hence remove it
                    connectedDeviceItr.remove();
                    Log.d(TAG,"Removing " + deviceAdd + " from connected Peer list");
                }
            }
        } else {
            Log.d(TAG,"No clients yet in the Group formed");
            mConnectedDevices.clear();
        }
    }

    private void onMultiSinkPeersChanged(Collection<NetDeviceIF> peers) {
		if (peers != null) {
			Map<String, NetDeviceIF> peersMap = new HashMap<String, NetDeviceIF>();
			for (NetDeviceIF dev: peers) {
				peersMap.put(dev.getAddress(), dev);
			}

            Iterator connectedDeviceItr = mConnectedDevices.entrySet().iterator();
            while (connectedDeviceItr.hasNext()) {
                Map.Entry entry = (Map.Entry) connectedDeviceItr.next();
                String deviceAdd = (String) entry.getKey();
                NetDeviceIF device = peersMap.get(deviceAdd);
                Log.d(TAG,"Searching for device add "+ deviceAdd);
                if(device == null) {
                    //This P2P device was in the connected device list but isn't
                    //anymore in the new Peer list, so it's gone, hence remove it
                    NetDeviceIF remDevice = mConnectedDevices.remove(deviceAdd);
                    Log.d(TAG,"Removing " + remDevice.getName() + " from connected Peer list");
                } else {
                    Log.d(TAG,"Device " + device.getName() + " is in Peer list");
                }
            }
        } else {
            Log.d(TAG,"device list is empty");
        }
    }
	/**
	 * Clears UI fields after a teardown
	 */
	public void doTeardown() {
		Log.d(TAG, "doTeardown() called");
		if (null != deviceDetailsFrameLayout &&
		        null != deviceDetailsFrameLayout.findViewById(R.id.btn_start_client))
		{
			deviceDetailsFrameLayout.findViewById(R.id.btn_start_client)
				.setVisibility(View.VISIBLE);
		} else {
			Log.d(TAG, "in doTeardown()- btn_start_client is null");
		}
	}

	/**
	 * Class for internal event handling. Must run on UI thread.
	 */
	class EventHandler extends Handler {
		private static final String TAG = "C.DeviceListFragment";

		@Override
		public void handleMessage(Message msg) {
			Log.d(TAG, "Event handler received: " + msg.what);
			switch (msg.what) {

			/* Timer events */

			case WLAN_EVENT_P2P_PEER_LIST_FINISH: {
				peers.clear();
				//WifiP2pDeviceList peerList = (WifiP2pDeviceList) msg.obj;
				Collection<NetDeviceIF> peerList = (Collection<NetDeviceIF>) msg.obj;
				for (NetDeviceIF peer : peerList) {
					Log.d(TAG, "WifiP2pDevice: " + peer.getAddress());
					peers.add(peer);
				}
				((WiFiPeerListAdapter) getListAdapter()).notifyDataSetChanged();
			}
				break;
			case connectionInfoAvailable: {
				//final WifiP2pInfo info = WifiP2pNetManager.sInstance.localDevice.mWifiP2pInfo;
				//final NetDeviceIF localDev = netManager.getLocalDevice();
				//final WifiP2pInfo info = ((WifiP2pNetDevice)localDev).getConnectionInfo();
				ConnectionInfoIF info = (ConnectionInfoIF)msg.obj;
				if (info == null) {
					throw new IllegalArgumentException("WifiP2pInfo shall not be null here");
				}

				Log.d(TAG, info.toString());
				if (deviceDetailsFrameLayout != null) {
					deviceDetailsFrameLayout.setVisibility(View.VISIBLE);
					deviceDetailsFrameLayout.findViewById(R.id.btn_disconnect)
							.setVisibility(View.VISIBLE);

					// The owner IP is now known.
					TextView view = (TextView) deviceDetailsFrameLayout
							.findViewById(R.id.tv_group_owner);
					view.setText(getResources().getString(
							R.string.group_owner_text)
							+ (info.isGroupOwner()? getResources().getString(R.string.yes)
							: getResources().getString(R.string.no)));
					view.setVisibility(View.VISIBLE);

					// InetAddress from WifiP2pInfo struct.
					view = (TextView) deviceDetailsFrameLayout
							.findViewById(R.id.tv_group_ip);
					view.setText("Group Owner IP - "
							+ info.getGroupOwnerIp());
					view.setVisibility(View.VISIBLE);

					WfdEnums.WFDDeviceType localDeviceType = ((DeviceActionListener)getActivity()).getLocalDeviceType();
								Log.d(TAG, "connectionInfoAvailable: localDeviceType- " + localDeviceType);
					// Don't show the start session button if the two
					// devices are of the same type
					deviceDetailsFrameLayout.findViewById(R.id.btn_start_client).setVisibility(View.VISIBLE);
                    if (ServiceUtil.getmServiceAlreadyBound()) {
						try {
							if (activity.getCurrentSession() == null) {
								deviceDetailsFrameLayout.findViewById(
										R.id.btn_start_client).setEnabled(true);
							}
						} catch (Exception e) {
							e.printStackTrace();
						}
                    } else {
						Log.e(TAG, "service is not bound");
					}
					//NetDeviceIF connectedDevice = whisperer.connectedDevice;
					if (connectedDevice.hasDeviceInfo() && connectedDevice.supportWfd()) {
						Log.d(TAG, "connectionInfoAvailable: connectedDevice wfdInfo is"/* + connectedDevice.wfdInfo*/);
						if (connectedDevice.getWfdType() == localDeviceType) {
							Log.d(TAG, "connectionInfoAvailable: localDeviceType- " + localDeviceType +
									", connectedDeviceType- " + connectedDevice.getWfdType());
							deviceDetailsFrameLayout.findViewById(R.id.btn_start_client).setEnabled(false);
						}
					}

					// hide the connect buttons
					deviceDetailsFrameLayout.findViewById(R.id.btn_connect)
							.setVisibility(View.GONE);
					deviceDetailsFrameLayout.findViewById(
							R.id.btn_connect_options).setVisibility(View.GONE);
                    if (whisperer.isMultiSinkMode()) {
                        View v = deviceDetailsFrameLayout.findViewById(R.id.btn_start_client);
                        Log.e(TAG,"ICHAKRAB Connecting straightaway");
                        Toast.makeText(getActivity(),"Connecting straightaway",Toast.LENGTH_SHORT).show();
                        handleStartClientClick(v);
                    }
				}
			}
				break;
			default:
				Log.e(TAG, "Unknown event received: " + msg.what);
			}
		}
	}

	/**
	 * An interface-callback for the activity to listen to fragment interaction
	 * events.
	 */
	public interface DeviceActionListener {

	    void cancelDisconnect();

	    void startSession(NetDeviceIF device);

	    void stopSession(int sessionId);

	    WfdEnums.WFDDeviceType getLocalDeviceType();

	    void peersReceived();
	}
}
